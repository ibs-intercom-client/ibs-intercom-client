/*
 * Copyright (C) 2021 Homegear GmbH
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "doorbell.h"
#include "doorbell_exception.h"
#include "doorbell_logger.h"
#include "doorbell_version.h"
#include "doorbell_appname.h"
#include "doorbell_command_line_handler.h"
#include <iostream>
#include "common/ringtone_player_setup_exception.h"
#include "gstreamer_media/gstreamer_ringtone_player.h"
#include "config/config_data.h"
#include "config/config_data_exception.h"
#include "config/doorbell_config_data_factory.h"
#include "common/severity_level.h"
#include "common/single_instance_lock.h"
#include "common/baselib_manager.h"

static const std::string APPLICATION_NAME = "doorbell";
static doorctrl::SingleInstanceLock single_instance_lock;

int main(int argc, char **argv) {

  doorctrl::ApplicationConfig application_config;

  application_config.app_version = doorctrl::DOORBELL_VERSION;
  application_config.app_name = doorctrl::DOORBELL_APPNAME;
  application_config.binary_name = doorctrl::DOORBELL_BINNAME;

  /*
   * parse command line options and return values as struct
   * help message and version info is printed on demand which 
   * will exit the process 
   */
  doorctrl::DoorbellCommandLineOptions options = doorctrl::DoorbellCommandLineHandler::handleArgs(argc, argv); 

  doorctrl::BaselibManager::getInstance().getSharedObjects()->debugLevel = options.console_debug + 1;

  if(!single_instance_lock.lock()) {
      doorctrl::DoorbellLogger::getInstance().getOutput().printCritical("Critical: doorbell is already running, pid file is locked or not writable");
      exit(1);
  }

  application_config.argc_ptr = &argc;
  application_config.argv_ptr = &argv;

  gst_init(application_config.argc_ptr, application_config.argv_ptr);

  /*
   * create single doorctrl instance as main application object
   */
  auto doorbell = std::make_shared<doorctrl::Doorbell>();

  doorctrl::Doorbell::printStartupMessage();

  std::shared_ptr<doorctrl::ConfigData> config_data;

  try {
     config_data = doorctrl::DoorbellConfigDataFactory::getInstance().getConfigData();
  } catch(doorctrl::ConfigDataException& ex) {
     doorctrl::DoorbellLogger::getInstance().getOutput().printCritical("Critical: error while fetching config data, message = " + ex.getMessage());
     exit(1); 
  }


  doorctrl::LoggingConfig log_config = config_data->getLoggingConfig();
  if(log_config.console_loglevel < options.console_debug) {
     log_config.console_loglevel = options.console_debug;
     config_data->updateLoggingConfig(log_config);
  }

  /*
   * homegear debug level starts with 1 = critical
   */
  if(log_config.console_loglevel > log_config.loglevel) {
     doorctrl::BaselibManager::getInstance().getSharedObjects()->debugLevel =
        config_data->getLoggingConfig().console_loglevel + 1;
  } else {
     if(log_config.logging_disabled) {
        doorctrl::BaselibManager::getInstance().getSharedObjects()->debugLevel = 0;
     } else {
        doorctrl::BaselibManager::getInstance().getSharedObjects()->debugLevel =
           config_data->getLoggingConfig().loglevel + 1;
     }
  }


  try {
     doorbell->setup(config_data);

     doorbell->getConfigData()->updateApplicationConfig(application_config);

     while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(60));
     }

  } catch (doorctrl::DoorbellException& ex) {
      doorctrl::DoorbellLogger::getInstance().getOutput().printCritical("Critical: a fatal error occurred, exiting...");
      doorbell->cleanup();
      exit(1);
  }

  doorbell->cleanup();
  single_instance_lock.unlock();
  exit(0);
}


void doorctrl::Doorbell::setup(std::shared_ptr<doorctrl::ConfigData>& config_data) {

  doorctrl::DoorbellLogger::getInstance().getOutput().printDebug("Debug: initializing " + APPLICATION_NAME);

  this->config_data = config_data;

  this->push_button_connector = std::make_shared<doorctrl::PushButtonConnector>();
  this->push_button_connector->setup(this->config_data);
  this->push_button_connector->addObserver(shared_from_this());
  this->push_button_connector->start();


  this->ringtone_player = std::make_shared<doorctrl::GStreamerRingtonePlayer>(this->config_data);

  try {
    this->ringtone_player->setup();
    this->ringtone_player->stopRinging();
  } catch(doorctrl::RingtonePlayerSetupException& ex) {
    doorctrl::DoorbellLogger::getInstance().getOutput().printCritical("Critical: cannot setup ringtone player, exiting...");
    this->ringtone_player->cleanup();
    this->ringtone_player.reset();


    exit(1);
  }

  if(this->config_data->getHealthConfig().enabled) {
    this->setupDoorbellHealth();
  }

  if(this->config_data->getHomegearRpcConfig().enabled) {
    this->setupHomegearRpcClient();
    this->setupDoorbellConfigDataConnector();

    if(this->config_data_connector != nullptr) {
       this->config_data_connector->doSingleConfigDataRead();
    }

//    this->config_data_connector->startDoorbellConfigMonitor();
  }
}

void doorctrl::Doorbell::setupHomegearRpcClient() {

  this->rpc_client = std::make_shared<doorctrl::RpcClient>();
  this->rpc_client->setup(this->config_data);
  this->rpc_event_connector = std::make_shared<doorctrl::RpcEventConnector>();
  this->rpc_event_connector->setup(this->rpc_client);
   
  this->rpc_client->start();

  int repeat_count = 0;

  while(!this->rpc_client->isConnected() && repeat_count < 10) {
     std::this_thread::sleep_for(std::chrono::milliseconds(200));
     repeat_count++;
  }

  if(!this->rpc_client->isConnected()) {

     this->rpc_client->stop();
     this->rpc_client->waitForStop();

     this->rpc_event_connector.reset();
     this->rpc_client.reset();

     doorctrl::DoorbellLogger::getInstance().getOutput().printError("Error: cannot start rpc client, running without rpc");
  }
}


void doorctrl::Doorbell::setupDoorbellConfigDataConnector() {

   if(this->rpc_client != nullptr) {
      this->config_data_connector = std::make_shared<DoorbellConfigDataConnector>();
      this->config_data_connector->setup(this->rpc_client, this->config_data);

      /*
       * this will start a polling loop to fetch config data from 
       * remote core module
       */

      /*
       * this->config_data_connector->startDoorbellConfigMonitor();
       */
      
      this->push_button_connector->addObserver(this->config_data_connector);
   }
}


void doorctrl::Doorbell::setupDoorbellHealth() {

  this->health_collector = std::make_shared<doorctrl::HealthCollector>();
  this->health_collector->setup(this->config_data);
  this->health_collector->start();
  
/*
  this->health_interface = std::make_shared<doorctrl::HttpServer>();
  this->health_interface->setup(this->config_data);
  std::shared_ptr<HealthHttpRequestHandler> handler(new HealthHttpRequestHandler("/health", this->health_collector));
  this->health_interface->addRequestHandler(handler);
*/

}



void doorctrl::Doorbell::cleanup() {

  if(this->ringtone_player != nullptr) {
    this->ringtone_player->cleanup();
  }   
}


std::shared_ptr<doorctrl::ConfigData> &doorctrl::Doorbell::getConfigData() {
  return this->config_data;
}


void doorctrl::Doorbell::handleEvent(const PushButtonStateEvent& event) {

  if(event.isPressed()) {

    if(this->config_data_connector != nullptr) {
       this->config_data_connector->doSingleConfigDataRead();
    }

    this->ringtone_player->ringOnce();
  }
}


void doorctrl::Doorbell::printStartupMessage() {
  std::cout << APPLICATION_NAME << " " << doorctrl::DOORBELL_VERSION << std::endl; 
  doorctrl::DoorbellLogger::getInstance().getOutput().printInfo("Info: " + APPLICATION_NAME + " " + doorctrl::DOORBELL_VERSION);
}
