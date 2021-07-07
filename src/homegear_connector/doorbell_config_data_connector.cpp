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

#include "doorbell_config_data_connector.h"
#include "homegear_connector_logger.h"
#include <iostream>


void doorctrl::DoorbellConfigDataConnector::setup(const std::shared_ptr<RpcClient>& client, const std::shared_ptr<ConfigData>& config_data) {
   this->client = client;
   this->config_data = config_data;

   this->doorbell_device_id = this->searchDoorbellDevice(); 
}


void doorctrl::DoorbellConfigDataConnector::startDoorbellConfigMonitor() {
  this->monitor_thread =
     std::make_unique<std::thread>(std::bind(&doorctrl::DoorbellConfigDataConnector::runDoorbellConfigMonitorLoop, this));
}


void doorctrl::DoorbellConfigDataConnector::runDoorbellConfigMonitorLoop() {

   while(true) {

      if(this->client->isConnected()) {

         if(this->doorbell_device_id == -1) {
            this->doorbell_device_id = this->searchDoorbellDevice(); 
         }

         this->readRemoteConfigData(); 
//         this->updateDoorbellDevice(); 
         std::this_thread::sleep_for(std::chrono::milliseconds(500));
      } else {
         /*
          * Wait a little bit longer when there is no connection. The 
          * RPC client is trying to reconnect, just sit and wait.
          */
         std::this_thread::sleep_for(std::chrono::milliseconds(2000));
      }
   }
}

void doorctrl::DoorbellConfigDataConnector::doSingleConfigDataRead() {

  if(this->doorbell_device_id == -1) {
     this->doorbell_device_id = this->searchDoorbellDevice();
  }
  this->updateDoorbellDevice();

  /*
   * sleep to give the homegear server some time to copy the
   * current user setting to a homegear device
   */
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  this->readRemoteConfigData();
}


void doorctrl::DoorbellConfigDataConnector::readRemoteConfigData() {
   doorctrl::HomegearConnectorLogger::getInstance().getOutput().printDebug("Debug: doorbell device id = " +  this->doorbell_device_id);

   this->updateDoorbellDevice();

   BaseLib::PVariable ringtone_volume = this->readDeviceVariable("RINGTONE_VOLUME");
   BaseLib::PVariable mute_bell = this->readDeviceVariable("MUTE_BELL");

   doorctrl::RingtoneConfig ringtone_config = this->config_data->getRingtoneConfig();

   if(ringtone_volume != nullptr) {
      ringtone_config.volume = ringtone_volume->integerValue;
   }

   if(mute_bell != nullptr) {
      ringtone_config.mute = mute_bell->booleanValue;
   }

   this->config_data->updateRingtoneConfig(ringtone_config);
}


BaseLib::PVariable doorctrl::DoorbellConfigDataConnector::readDeviceVariable(const std::string& variable_name) {
   if(this->doorbell_device_id != -1) {
      std::lock_guard<std::mutex> lock(this->config_data_mutex);

      BaseLib::PArray params = std::make_shared<BaseLib::Array>();
      params->push_back(std::make_shared<BaseLib::Variable>(this->doorbell_device_id));
      params->push_back(std::make_shared<BaseLib::Variable>(1));
      params->push_back(std::make_shared<BaseLib::Variable>(variable_name.c_str()));
      std::shared_ptr<BaseLib::Variable> result = this->client->invoke("getValue", params, 1000);
      return result;
   } else {
     return nullptr; 
   }
}

void doorctrl::DoorbellConfigDataConnector::updateDoorbellDevice() {
   if(this->doorbell_device_id != -1) {
      BaseLib::PArray params = std::make_shared<BaseLib::Array>();
      params->push_back(std::make_shared<BaseLib::Variable>(this->doorbell_device_id));
      params->push_back(std::make_shared<BaseLib::Variable>(1));
      params->push_back(std::make_shared<BaseLib::Variable>("UPDATE_CONFIG"));
      params->push_back(std::make_shared<BaseLib::Variable>(1));
      params->push_back(std::make_shared<BaseLib::Variable>(true));

      std::shared_ptr<BaseLib::Variable> result = this->client->invoke("setValue", params, 1000);
   }
}


void doorctrl::DoorbellConfigDataConnector::writeDeviceVariable(const std::string& variable_name, BaseLib::PVariable value) {
   if(this->doorbell_device_id != -1) {
      BaseLib::PArray params = std::make_shared<BaseLib::Array>();
      params->push_back(std::make_shared<BaseLib::Variable>(this->doorbell_device_id));
      params->push_back(std::make_shared<BaseLib::Variable>(1));
      params->push_back(std::make_shared<BaseLib::Variable>(variable_name.c_str()));
      params->push_back(value);
      params->push_back(std::make_shared<BaseLib::Variable>(true));

      std::shared_ptr<BaseLib::Variable> result = this->client->invoke("setValue", params, 1000);
   }
}


void doorctrl::DoorbellConfigDataConnector::handleEvent(const PushButtonStateEvent& event) {

   doorctrl::HomegearConnectorLogger::getInstance().getOutput().printDebug("Debug: handle push button state event, pushed = " + std::to_string(event.isPressed()));
   BaseLib::PVariable pushed = std::make_shared<BaseLib::Variable>(event.isPressed());
   this->writeDeviceVariable("STATE", pushed);
}


int32_t doorctrl::DoorbellConfigDataConnector::searchDoorbellDevice() {
   BaseLib::PArray fields = std::make_shared<BaseLib::Array>();
   fields->push_back(std::make_shared<BaseLib::Variable>("ID"));
   fields->push_back(std::make_shared<BaseLib::Variable>("TYPE"));

   BaseLib::PArray params = std::make_shared<BaseLib::Array>();
   params->push_back(std::make_shared<BaseLib::Variable>(false));
   params->push_back(std::make_shared<BaseLib::Variable>(fields));
   params->push_back(std::make_shared<BaseLib::Variable>(254));
   std::shared_ptr<BaseLib::Variable> result = this->client->invoke("listDevices", params, 1000);

   BaseLib::PArray devices = result->arrayValue; 

   doorctrl::HomegearRpcConfig rpc_config = this->config_data->getHomegearRpcConfig();


   for(auto current_device : *devices) {

      if(current_device->structValue->find("TYPE")->second->stringValue == rpc_config.doorbell_device_id) {
         doorctrl::HomegearConnectorLogger::getInstance().getOutput().printDebug("Debug: found doorbell device id = " +  std::to_string(current_device->structValue->find("ID")->second->integerValue));
         return current_device->structValue->find("ID")->second->integerValue;
      }
   }

   doorctrl::HomegearConnectorLogger::getInstance().getOutput().printError("Error: doorbell device " + rpc_config.doorbell_device_id +  " not found, could not get config data from homegear");
   return -1; 
}

