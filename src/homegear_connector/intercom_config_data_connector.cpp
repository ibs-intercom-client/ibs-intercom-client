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

#include "intercom_config_data_connector.h"
#include "homegear_connector_logger.h"
#include <iostream>


void doorctrl::IntercomConfigDataConnector::setup(const std::shared_ptr<RpcClient>& client, const std::shared_ptr<ConfigData>& config_data) {
   this->client = client;
   this->config_data = config_data;

   this->intercom_device_id = this->searchIntercomDevice(); 
}


void doorctrl::IntercomConfigDataConnector::startIntercomConfigMonitor() {
  this->monitor_thread =
     std::make_unique<std::thread>(std::bind(&doorctrl::IntercomConfigDataConnector::runIntercomConfigMonitorLoop, this));
}


void doorctrl::IntercomConfigDataConnector::runIntercomConfigMonitorLoop() {

   while(true) {

      if(this->client->isConnected()) {

         if(this->intercom_device_id == -1) {
            this->intercom_device_id = this->searchIntercomDevice(); 
         }

         this->readRemoteConfigData(); 
//         this->updateIntercomDevice(); 
         std::this_thread::sleep_for(std::chrono::milliseconds(50));
      } else {
         /*
          * Wait a little bit longer when there is no connection. The 
          * RPC client is trying to reconnect, just sit and wait.
          */
         std::this_thread::sleep_for(std::chrono::milliseconds(2000));
      }
   }
}


void doorctrl::IntercomConfigDataConnector::startSingleConfigDataRead() {
  doorctrl::HomegearConnectorLogger::getInstance().getOutput().printDebug("Debug: start single config data read");

  if(this->single_read_thread != nullptr) {
     this->single_read_thread->join(); 
  }

  this->single_read_thread =
     std::make_unique<std::thread>(std::bind(&doorctrl::IntercomConfigDataConnector::doSingleConfigDataRead, this));
}


void doorctrl::IntercomConfigDataConnector::doSingleConfigDataRead() {

  if(this->intercom_device_id == -1) {
     this->intercom_device_id = this->searchIntercomDevice(); 
  }
  this->updateIntercomDevice();

  /*
   * sleep to give the homegear server some time to copy the
   * current user setting to a homegear device
   */
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  this->readRemoteConfigData();
}


void doorctrl::IntercomConfigDataConnector::readRemoteConfigData() {

   if(this->client->isConnected()) {
      this->updateIntercomDevice();

      BaseLib::PVariable mute_bell = this->readDeviceVariable("MUTE_BELL");
      BaseLib::PVariable ringtone_volume = this->readDeviceVariable("RINGTONE_VOLUME");
      BaseLib::PVariable speaker_volume = this->readDeviceVariable("SPEAKER_VOLUME");
      BaseLib::PVariable mic_level = this->readDeviceVariable("MICROPHONE_LEVEL");
      BaseLib::PVariable highlight_color = this->readDeviceVariable("HIGHLIGHT_COLOR");

      doorctrl::PhoneConfig phone_config = this->config_data->getPhoneConfig();

      if(speaker_volume != nullptr) {
         phone_config.volume = speaker_volume->integerValue;
      }

      if(mic_level != nullptr) {
         phone_config.mic_level = mic_level->integerValue;
      }

      this->config_data->updatePhoneConfig(phone_config);


      doorctrl::RingtoneConfig ringtone_config = this->config_data->getRingtoneConfig();

      if(ringtone_volume != nullptr) {
         ringtone_config.volume = ringtone_volume->integerValue;
      }

      if(mute_bell != nullptr) {
         ringtone_config.mute = mute_bell->booleanValue;
      }

      this->config_data->updateRingtoneConfig(ringtone_config);


      doorctrl::UiConfig ui_config = this->config_data->getUiConfig();

      if(highlight_color != nullptr) {
         ui_config.highlight_color = highlight_color->stringValue;
      }

      this->config_data->updateUiConfig(ui_config);
   }
}


BaseLib::PVariable doorctrl::IntercomConfigDataConnector::readDeviceVariable(const std::string& variable_name) {
   if(this->intercom_device_id != -1) {
      std::lock_guard<std::mutex> lock(this->config_data_mutex);

      BaseLib::PArray params = std::make_shared<BaseLib::Array>();
      params->push_back(std::make_shared<BaseLib::Variable>(this->intercom_device_id));
      params->push_back(std::make_shared<BaseLib::Variable>(1));
      params->push_back(std::make_shared<BaseLib::Variable>(variable_name.c_str()));
      std::shared_ptr<BaseLib::Variable> result = this->client->invoke("getValue", params, 1000);
//      result->print(true);
      return result;
   } else {
     return nullptr; 
   }
}

void doorctrl::IntercomConfigDataConnector::updateIntercomDevice() {
   if(this->intercom_device_id != -1) {
      BaseLib::PArray params = std::make_shared<BaseLib::Array>();
      params->push_back(std::make_shared<BaseLib::Variable>(this->intercom_device_id));
      params->push_back(std::make_shared<BaseLib::Variable>(1));
      params->push_back(std::make_shared<BaseLib::Variable>("UPDATE_CONFIG"));
      params->push_back(std::make_shared<BaseLib::Variable>(1));
      params->push_back(std::make_shared<BaseLib::Variable>(true));

      std::shared_ptr<BaseLib::Variable> result = this->client->invoke("setValue", params, 1000);
//      result->print(true);
   }
}


void doorctrl::IntercomConfigDataConnector::writeDeviceVariable(const std::string& variable_name, BaseLib::PVariable value) {
   if(this->intercom_device_id != -1) {
      BaseLib::PArray params = std::make_shared<BaseLib::Array>();
      params->push_back(std::make_shared<BaseLib::Variable>(this->intercom_device_id));
      params->push_back(std::make_shared<BaseLib::Variable>(1));
      params->push_back(std::make_shared<BaseLib::Variable>(variable_name.c_str()));
      params->push_back(value);
      params->push_back(std::make_shared<BaseLib::Variable>(true));

      std::shared_ptr<BaseLib::Variable> result = this->client->invoke("setValue", params, 1000);
//      result->print(true);
   }
}


int32_t doorctrl::IntercomConfigDataConnector::searchIntercomDevice() {
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

      if(current_device->structValue->find("TYPE")->second->stringValue == rpc_config.intercom_device_id) {
         doorctrl::HomegearConnectorLogger::getInstance().getOutput().printDebug("Debug: found intercom device id = " + std::to_string(current_device->structValue->find("ID")->second->integerValue));
         return current_device->structValue->find("ID")->second->integerValue;
      }
   }

   doorctrl::HomegearConnectorLogger::getInstance().getOutput().printError("Error: intercom device " + rpc_config.intercom_device_id + " not found, could not get config data from homegear");
   return -1; 
}


void doorctrl::IntercomConfigDataConnector::saveConfigData(const std::shared_ptr<ConfigData>& config_data) {

   PhoneConfig phone_config = config_data->getPhoneConfig();

   BaseLib::PVariable speaker_volume = std::make_shared<BaseLib::Variable>(phone_config.volume);
   this->writeDeviceVariable("SPEAKER_VOLUME", speaker_volume);
}

