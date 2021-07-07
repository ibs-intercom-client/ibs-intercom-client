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

#include "config_data.h"
#include "common/simple_observer_manager.cpp"
#include "config_logger.h"

const doorctrl::SipConfig& doorctrl::ConfigData::getSipConfig() const {
   return this->sip_config;  
}


void doorctrl::ConfigData::updateSipConfig(const SipConfig& modified_sip_config) {

   if(!this->sip_config.equals(modified_sip_config)) {
     doorctrl::ConfigLogger::getInstance().getOutput().printDebug("Debug: updating sip config data");
     this->sip_config = modified_sip_config;
     this->observer_manager.fireEvent(doorctrl::ConfigDataChangedEvent::getSipConfigChangedEvent());
   }
}


const doorctrl::RingtoneConfig& doorctrl::ConfigData::getRingtoneConfig() const {
   return this->ringtone_config;  
}


void doorctrl::ConfigData::updateRingtoneConfig(const RingtoneConfig& modified_ringtone_config) {

   if(!this->ringtone_config.equals(modified_ringtone_config)) {
     doorctrl::ConfigLogger::getInstance().getOutput().printDebug("Debug: updating ringtone player config data");
     this->ringtone_config = modified_ringtone_config;
     this->observer_manager.fireEvent(doorctrl::ConfigDataChangedEvent::getRingtoneConfigChangedEvent());
   }
}


const doorctrl::VideoConfig& doorctrl::ConfigData::getVideoConfig() const {
   return this->video_config;  
}


void doorctrl::ConfigData::updateVideoConfig(const VideoConfig& modified_video_config) {

   if(!this->video_config.equals(modified_video_config)) {
     doorctrl::ConfigLogger::getInstance().getOutput().printDebug("Debug: updating video config data");
     this->video_config = modified_video_config;
     this->observer_manager.fireEvent(doorctrl::ConfigDataChangedEvent::getVideoConfigChangedEvent());
   }
}


const doorctrl::LoggingConfig& doorctrl::ConfigData::getLoggingConfig() const {
   return this->logging_config;  
}


void doorctrl::ConfigData::updateLoggingConfig(const LoggingConfig& modified_logging_config) {

   if(!this->logging_config.equals(modified_logging_config)) {
     doorctrl::ConfigLogger::getInstance().getOutput().printDebug("Debug: updating logging config data");
     this->logging_config = modified_logging_config;
     this->observer_manager.fireEvent(doorctrl::ConfigDataChangedEvent::getLoggingConfigChangedEvent());
   }
}


const doorctrl::PhoneConfig& doorctrl::ConfigData::getPhoneConfig() const {
   return this->phone_config;  
}


void doorctrl::ConfigData::updatePhoneConfig(const PhoneConfig& modified_phone_config) {

   if(!this->phone_config.equals(modified_phone_config)) {
     doorctrl::ConfigLogger::getInstance().getOutput().printDebug("Debug: updating phone config data");
     this->phone_config = modified_phone_config;
     this->observer_manager.fireEvent(doorctrl::ConfigDataChangedEvent::getPhoneConfigChangedEvent());
   }
}


const doorctrl::PushButtonConfig& doorctrl::ConfigData::getPushButtonConfig() const {
   return this->push_button_config;  
}

void doorctrl::ConfigData::updatePushButtonConfig(const PushButtonConfig& modified_push_button_config) {

   if(!this->push_button_config.equals(modified_push_button_config)) {
     doorctrl::ConfigLogger::getInstance().getOutput().printDebug("Debug: updating push button config data");
     this->push_button_config = modified_push_button_config;
     this->observer_manager.fireEvent(doorctrl::ConfigDataChangedEvent::getPushButtonConfigChangedEvent());
   }
}


const doorctrl::HomeButtonConfig& doorctrl::ConfigData::getHomeButtonConfig() const {
   return this->home_button_config;  
}


void doorctrl::ConfigData::updateHomeButtonConfig(const HomeButtonConfig& modified_home_button_config) {

   if(!this->home_button_config.equals(modified_home_button_config)) {
     doorctrl::ConfigLogger::getInstance().getOutput().printDebug("Debug: updating home button config data");
     this->home_button_config = modified_home_button_config;
     this->observer_manager.fireEvent(doorctrl::ConfigDataChangedEvent::getHomeButtonConfigChangedEvent());
   }
}


const doorctrl::ApplicationConfig& doorctrl::ConfigData::getApplicationConfig() const {
   return this->application_config;  
}


void doorctrl::ConfigData::updateApplicationConfig(const ApplicationConfig& modified_application_config) {

   if(!this->application_config.equals(modified_application_config)) {
     doorctrl::ConfigLogger::getInstance().getOutput().printDebug("Debug: updating application config data");
     this->application_config = modified_application_config;
     this->observer_manager.fireEvent(doorctrl::ConfigDataChangedEvent::getApplicationConfigChangedEvent());
   }
}


const doorctrl::HealthConfig& doorctrl::ConfigData::getHealthConfig() const {
   return this->health_config;  
}


void doorctrl::ConfigData::updateHealthConfig(const HealthConfig& modified_health_config) {

   if(!this->health_config.equals(modified_health_config)) {
     doorctrl::ConfigLogger::getInstance().getOutput().printDebug("Debug: updating health config data");
     this->health_config = modified_health_config;
     this->observer_manager.fireEvent(doorctrl::ConfigDataChangedEvent::getHealthConfigChangedEvent());
   }
}


const doorctrl::HomegearRpcConfig& doorctrl::ConfigData::getHomegearRpcConfig() const {
   return this->homegear_rpc_config;  
}


void doorctrl::ConfigData::updateHomegearRpcConfig(const HomegearRpcConfig& modified_homegear_rpc_config) {

   if(!this->homegear_rpc_config.equals(modified_homegear_rpc_config)) {
     doorctrl::ConfigLogger::getInstance().getOutput().printDebug("Debug: updating homegear_rpc config data");
     this->homegear_rpc_config = modified_homegear_rpc_config;
     this->observer_manager.fireEvent(doorctrl::ConfigDataChangedEvent::getHomegearRpcConfigChangedEvent());
   }
}


const doorctrl::UiConfig& doorctrl::ConfigData::getUiConfig() const {
   return this->ui_config;  
}


void doorctrl::ConfigData::updateUiConfig(const UiConfig& modified_ui_config) {

   if(!this->ui_config.equals(modified_ui_config)) {
     doorctrl::ConfigLogger::getInstance().getOutput().printDebug("Debug: updating ui config data");
     this->ui_config = modified_ui_config;
     this->observer_manager.fireEvent(doorctrl::ConfigDataChangedEvent::getUiConfigChangedEvent());
   }
}


void doorctrl::ConfigData::addObserver(const std::shared_ptr<ConfigDataChangedObserver>& observer) {
   this->observer_manager.addObserver(observer);
}


void doorctrl::ConfigData::removeObserver(const std::shared_ptr<ConfigDataChangedObserver>& observer) {
   this->observer_manager.removeObserver(observer);
}


void doorctrl::ConfigData::save() {

  for(const auto & persistence : this->persistence_list) {
     persistence->saveConfigData(shared_from_this()); 
  }
   
}


void doorctrl::ConfigData::addConfigDataPersistence(const std::shared_ptr<ConfigDataPersistence>& persistence) {
   this->persistence_list.push_back(persistence);
}
