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

#include "doorctrl.h"
#include "doorctrl_exception.h"
#include "doorctrl_logger.h"
#include "doorctrl_version.h"
#include "doorctrl_appname.h"
#include "doorctrl_command_line_handler.h"
#include <gst/gst.h>
#include <iostream>
#include "gtk_ui/gtk_ui.h"
#include "pjsip_phone/pjsip_phone.h"
#include "common/ringtone_player_setup_exception.h"
#include "common/null_ringtone_player.h"
#include "common/phone.h"
#include "common/phone_setup_exception.h"
#include "common/door_opener.h"
#include "gstreamer_media/gstreamer_ringtone_player.h"
#include "config/config_data.h"
#include "config/config_data_exception.h"
#include "config/doorctrl_config_data_factory.h"
#include "common/severity_level.h"
#include "http_interface/health_http_request_handler.h"
#include "common/baselib_manager.h"

static const std::string APPLICATION_NAME = "doorctrl";
static int empty_argc = 0;
static char** empty_argv = {};


int main(int argc, char **argv) {

  doorctrl::ApplicationConfig application_config;

  application_config.kiosk_mode = false;
  application_config.app_version = doorctrl::DOORCTRL_VERSION;
  application_config.app_name = doorctrl::DOORCTRL_APPNAME;
  application_config.binary_name = doorctrl::DOORCTRL_BINNAME;

  /*
   * command line args are parsed by the doorctrl application itself. No need to
   * include gtk3+ into this process.
   * Gtk3+ default command line options are disabled this way.
   */
//  application_config.argc_ptr = &argc;
//  application_config.argv_ptr = &argv;

  application_config.argc_ptr = &empty_argc;
  application_config.argv_ptr = &empty_argv;

  /*
   * parse command line options and return values as struct
   */
  doorctrl::DoorctrlCommandLineOptions options = doorctrl::DoorctrlCommandLineHandler::handleArgs(argc, argv); 

  doorctrl::BaselibManager::getInstance().getSharedObjects()->debugLevel = options.console_debug + 1;


  application_config.kiosk_mode = options.kiosk_mode;


  gst_init(application_config.argc_ptr, application_config.argv_ptr);

  /*
   * create single doorctrl instance as main application object
   */
  auto doorctrl = std::make_shared<doorctrl::Doorctrl>();
  doorctrl::Doorctrl::printStartupMessage();
  std::shared_ptr<doorctrl::ConfigData> config_data;

  try {
    config_data = doorctrl::DoorctrlConfigDataFactory::getInstance().getConfigData();
  } catch(doorctrl::ConfigDataException& ex) {
     doorctrl::DoorctrlLogger::getInstance().getOutput().printError("Error: error while fetching config data, message = " + ex.getMessage());
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

  config_data->updateApplicationConfig(application_config);

  try {
     doorctrl->setup(config_data);

     /*
      * use DTMF over SIP provided by phone object
      */
      doorctrl::GtkUi::setup(doorctrl->getPhone(),
                             doorctrl->getDoorOpener(),
                             doorctrl->getHomeButton(),
                             doorctrl->getConfigData());


      doorctrl::GtkUi::enterMainLoop(*application_config.argc_ptr, 
                                     *application_config.argv_ptr);

  } catch (doorctrl::DoorctrlException& ex) {
      doorctrl::DoorctrlLogger::getInstance().getOutput().printCritical("Critical: a fatal error occurred, exiting...");
      doorctrl->cleanup();
      exit(1);
  }

  doorctrl->cleanup();
  exit(0);
}


void doorctrl::Doorctrl::setup(std::shared_ptr<doorctrl::ConfigData>& config_data) {

  doorctrl::DoorctrlLogger::getInstance().getOutput().printDebug("Debug: initializing " + APPLICATION_NAME);

  this->config_data = config_data;
/*
  try {
    this->config_data = doorctrl::DoorctrlConfigDataFactory::getInstance().getConfigData();
  } catch(ConfigDataException& ex) {
     doorctrl::DoorctrlLogger::getInstance().getOutput().printError("Error: error while fetching config data, message = " + ex.getMessage());
     throw DoorctrlException("doorctrl setup error");
  }

  doorctrl::LoggingConfig log_config = doorctrl->getConfigData()->getLoggingConfig();
  log_config.console_loglevel = options.console_debug;
  doorctrl->getConfigData()->updateLoggingConfig(log_config);

  doorctrl::BaselibManager::getInstance().getSharedObjects()->debugLevel =
     doorctrl->getConfigData()->getLoggingConfig().console_loglevel;

  std::cout << "loglevel = " << doorctrl->getConfigData()->getLoggingConfig().console_loglevel << std::endl;
*/


  if(this->config_data->getHomegearRpcConfig().enabled) {
    this->setupHomegearRpcClient();
    this->setupIntercomConfigDataConnector();
    this->config_data_connector->readRemoteConfigData();

    if(this->config_data_connector != nullptr) {
       this->config_data_connector->doSingleConfigDataRead();
    }
  }

  std::shared_ptr<doorctrl::HomeButtonConnector> home_button_connector = std::make_shared<doorctrl::HomeButtonConnector>();
  home_button_connector->setup(this->config_data);
  home_button_connector->start();
  this->home_button = home_button_connector;


  this->reboot_handler = std::make_shared<doorctrl::RebootHandler>(); 
  home_button_connector->addObserver(this->reboot_handler); 


  this->ringtone_player = std::make_shared<doorctrl::GStreamerRingtonePlayer>(this->config_data);

  doorctrl::DoorctrlLogger::getInstance().getOutput().printDebug("Debug: creating phone");
  auto pjsip_phone = std::make_shared<doorctrl::PjsipPhone>(this->config_data);
  this->phone = pjsip_phone;
  this->door_opener = pjsip_phone;

  (static_cast<doorctrl::ObservedSubject<doorctrl::PhoneCallStateObserver>& >(*pjsip_phone)).addObserver(shared_from_this());
  (static_cast<doorctrl::ObservedSubject<doorctrl::PhoneRegistrationStateObserver>* >(pjsip_phone.get()))->addObserver(shared_from_this());

  doorctrl::DoorctrlLogger::getInstance().getOutput().printDebug("Debug: calling phone setup");
  try {
    this->phone->setup();
  } catch(doorctrl::PhoneSetupException& ex) {
    doorctrl::DoorctrlLogger::getInstance().getOutput().printCritical("Critical: cannot setup phone, exiting...");
    this->phone->cleanup();
    this->phone.reset();
    exit(1);
  }

  try {
    this->ringtone_player->setup();
  } catch(doorctrl::RingtonePlayerSetupException& ex) {
    doorctrl::DoorctrlLogger::getInstance().getOutput().printCritical("Critical: cannot setup ringtone player, exiting...");

    this->ringtone_player->cleanup();
    this->ringtone_player.reset();
    this->phone->cleanup();
    this->phone.reset();
    exit(1);
  }

  doorctrl::DoorctrlLogger::getInstance().getOutput().printDebug("Debug: trying to register phone at phone server");

  try {
    this->phone->registerPhone();
  } catch(doorctrl::PhoneSetupException& ex) {
    doorctrl::DoorctrlLogger::getInstance().getOutput().printCritical("Critical: cannot register phone, exiting...");
    this->phone->cleanup();
    this->phone.reset();
    exit(1);
  }

  if(this->config_data->getHealthConfig().enabled) {
    this->setupDoorctrlHealth();
  }

  this->enterInitialState();

}

void doorctrl::Doorctrl::setupHomegearRpcClient() {

  this->rpc_client = std::make_shared<doorctrl::RpcClient>();
  this->rpc_client->setup(this->config_data);
  this->rpc_client->start();

  int repeat_count = 0;

  while(!this->rpc_client->isConnected() && repeat_count < 10) {
     std::this_thread::sleep_for(std::chrono::milliseconds(200));
     repeat_count++;
  }

  if(!this->rpc_client->isConnected()) {

//     this->rpc_client->stop();
//     this->rpc_client->waitForStop();

//     this->rpc_client.reset();

     doorctrl::DoorctrlLogger::getInstance().getOutput().printError("error: cannot start rpc client, running without rpc");
  } else {
     this->rpc_event_connector = std::make_shared<doorctrl::RpcEventConnector>();
     this->rpc_event_connector->setup(this->rpc_client);
  }
}


void doorctrl::Doorctrl::setupIntercomConfigDataConnector() {

   if(this->rpc_client != nullptr) {
      this->config_data_connector = std::make_shared<IntercomConfigDataConnector>();

      this->config_data_connector->setup(this->rpc_client, this->config_data);
      this->config_data->addConfigDataPersistence(this->config_data_connector);

      /*  
       * this would start a polling loop to fetch remote config data from
       * core module
       */
      /*
       * this->config_data_connector->startIntercomConfigMonitor();
       */
   }
}


void doorctrl::Doorctrl::setupDoorctrlHealth() {

  this->health_collector = std::make_shared<doorctrl::HealthCollector>();
  this->health_collector->setup(this->config_data);
  this->health_collector->start();
  
  this->health_interface = std::make_shared<doorctrl::HttpServer>();
  this->health_interface->setup(this->config_data);
  std::shared_ptr<HealthHttpRequestHandler> handler(new HealthHttpRequestHandler("/health", this->health_collector));
  this->health_interface->addRequestHandler(handler);

}



void doorctrl::Doorctrl::cleanup() {

  if(this->phone != nullptr) {
    (static_cast<doorctrl::ObservedSubject<doorctrl::PhoneCallStateObserver>* >(this->phone.get()))->removeObserver(shared_from_this());
    (static_cast<doorctrl::ObservedSubject<doorctrl::PhoneRegistrationStateObserver>* >(this->phone.get()))->removeObserver(shared_from_this());
    this->phone->cleanup();
  }

  if(this->ringtone_player != nullptr) {
    this->ringtone_player->cleanup();
  }   

  if(this->door_opener != nullptr) {
    this->door_opener->cleanup();
  }   
}

void doorctrl::Doorctrl::enterInitialState() {

  if (this->phone->hasIncomingCall()) {
    this->ringtone_player->startRinging();
  } else {
    this->enterIdleState();
  }
}

const std::string &doorctrl::Doorctrl::getRegisteredUserName() const {
  return this->phone->getRegisteredUserName();
}

void doorctrl::Doorctrl::handleEvent(const doorctrl::PhoneCallStateEvent& event) {
  switch(event.getCurrentCallState()) {

    case PHONE_CALL_STATE_RINGING:
        doorctrl::DoorctrlLogger::getInstance().getOutput().printDebug("Debug: handle");

        if(this->config_data_connector != nullptr) {

        doorctrl::DoorctrlLogger::getInstance().getOutput().printDebug("Debug: read remote config data");
        this->config_data_connector->startSingleConfigDataRead();

        std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
      } 
      this->ringtone_player->startRinging();
      break;
       
    default:
      this->ringtone_player->stopRinging();
  }
}


void doorctrl::Doorctrl::handleEvent(const doorctrl::PhoneRegistrationStateEvent& event) {
  if(event.getCurrentRegistrationState() == doorctrl::PHONE_REGISTRATION_STATE_AUTH_ERROR) {
     doorctrl::DoorctrlLogger::getInstance().getOutput().printCritical("Critical: cannot authenticate sip user, exiting...");
     exit(1);
  }
}

void doorctrl::Doorctrl::enterIdleState() {
  this->ringtone_player->stopRinging();
}

std::shared_ptr<doorctrl::Phone> &doorctrl::Doorctrl::getPhone() {
  return this->phone;
}

std::shared_ptr<doorctrl::DoorOpener> &doorctrl::Doorctrl::getDoorOpener() {
  return this->door_opener;
}

std::shared_ptr<doorctrl::HomeButton> &doorctrl::Doorctrl::getHomeButton() {
   return this->home_button;
}

std::shared_ptr<doorctrl::ConfigData> &doorctrl::Doorctrl::getConfigData() {
  return this->config_data;
}

void doorctrl::Doorctrl::printStartupMessage() {
  std::cout << APPLICATION_NAME << " " << doorctrl::DOORCTRL_VERSION << std::endl; 
  doorctrl::DoorctrlLogger::getInstance().getOutput().printInfo("Info: " + APPLICATION_NAME + " " + doorctrl::DOORCTRL_VERSION);
}
