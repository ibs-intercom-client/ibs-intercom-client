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

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "config_data_changed_event.h"

const std::string doorctrl::ConfigDataChangedEvent::SIP_SECTION_NAME = "sip";
const std::string doorctrl::ConfigDataChangedEvent::RINGTONE_SECTION_NAME = "ringtone";
const std::string doorctrl::ConfigDataChangedEvent::VIDEO_SECTION_NAME = "video";
const std::string doorctrl::ConfigDataChangedEvent::LOGGING_SECTION_NAME = "logging";
const std::string doorctrl::ConfigDataChangedEvent::PHONE_SECTION_NAME = "phone";
const std::string doorctrl::ConfigDataChangedEvent::PUSH_BUTTON_SECTION_NAME = "push_button";
const std::string doorctrl::ConfigDataChangedEvent::HOME_BUTTON_SECTION_NAME = "home_button";
const std::string doorctrl::ConfigDataChangedEvent::APPLICATION_SECTION_NAME = "application";
const std::string doorctrl::ConfigDataChangedEvent::HEALTH_SECTION_NAME = "health";
const std::string doorctrl::ConfigDataChangedEvent::HOMEGEAR_RPC_SECTION_NAME = "homegear-rpc";
const std::string doorctrl::ConfigDataChangedEvent::UI_SECTION_NAME = "ui";

static doorctrl::ConfigDataChangedEvent g_sip_config_changed_event(doorctrl::ConfigDataChangedEvent::SIP_SECTION_NAME);
static doorctrl::ConfigDataChangedEvent g_ringtone_config_changed_event(doorctrl::ConfigDataChangedEvent::RINGTONE_SECTION_NAME);
static doorctrl::ConfigDataChangedEvent g_video_config_changed_event(doorctrl::ConfigDataChangedEvent::VIDEO_SECTION_NAME);
static doorctrl::ConfigDataChangedEvent g_logging_config_changed_event(doorctrl::ConfigDataChangedEvent::LOGGING_SECTION_NAME);
static doorctrl::ConfigDataChangedEvent g_phone_config_changed_event(doorctrl::ConfigDataChangedEvent::PHONE_SECTION_NAME);
static doorctrl::ConfigDataChangedEvent g_push_button_config_changed_event(doorctrl::ConfigDataChangedEvent::PUSH_BUTTON_SECTION_NAME);
static doorctrl::ConfigDataChangedEvent g_home_button_config_changed_event(doorctrl::ConfigDataChangedEvent::HOME_BUTTON_SECTION_NAME);
static doorctrl::ConfigDataChangedEvent g_application_config_changed_event(doorctrl::ConfigDataChangedEvent::APPLICATION_SECTION_NAME);
static doorctrl::ConfigDataChangedEvent g_health_config_changed_event(doorctrl::ConfigDataChangedEvent::HEALTH_SECTION_NAME);
static doorctrl::ConfigDataChangedEvent g_homegear_rpc_config_changed_event(doorctrl::ConfigDataChangedEvent::HOMEGEAR_RPC_SECTION_NAME);
static doorctrl::ConfigDataChangedEvent g_ui_config_changed_event(doorctrl::ConfigDataChangedEvent::UI_SECTION_NAME);


doorctrl::ConfigDataChangedEvent::ConfigDataChangedEvent(const std::string& data_section) 
  : changed_data_section(data_section) {

}


const doorctrl::ConfigDataChangedEvent& doorctrl::ConfigDataChangedEvent::getSipConfigChangedEvent() {
  return g_sip_config_changed_event;
}


const doorctrl::ConfigDataChangedEvent& doorctrl::ConfigDataChangedEvent::getRingtoneConfigChangedEvent() {
  return g_ringtone_config_changed_event;
}


const doorctrl::ConfigDataChangedEvent& doorctrl::ConfigDataChangedEvent::getVideoConfigChangedEvent() {
  return g_video_config_changed_event;
}


const doorctrl::ConfigDataChangedEvent& doorctrl::ConfigDataChangedEvent::getLoggingConfigChangedEvent() {
  return g_logging_config_changed_event;
}


const doorctrl::ConfigDataChangedEvent& doorctrl::ConfigDataChangedEvent::getPhoneConfigChangedEvent() {
  return g_phone_config_changed_event;
}


const doorctrl::ConfigDataChangedEvent& doorctrl::ConfigDataChangedEvent::getPushButtonConfigChangedEvent() {
  return g_push_button_config_changed_event;
}


const doorctrl::ConfigDataChangedEvent& doorctrl::ConfigDataChangedEvent::getHomeButtonConfigChangedEvent() {
  return g_home_button_config_changed_event;
}


const doorctrl::ConfigDataChangedEvent& doorctrl::ConfigDataChangedEvent::getApplicationConfigChangedEvent() {
  return g_application_config_changed_event;
}


const doorctrl::ConfigDataChangedEvent& doorctrl::ConfigDataChangedEvent::getHealthConfigChangedEvent() {
  return g_health_config_changed_event;
}


const doorctrl::ConfigDataChangedEvent& doorctrl::ConfigDataChangedEvent::getHomegearRpcConfigChangedEvent() {
  return g_homegear_rpc_config_changed_event;
}


const doorctrl::ConfigDataChangedEvent& doorctrl::ConfigDataChangedEvent::getUiConfigChangedEvent() {
  return g_ui_config_changed_event;
}


const std::string& doorctrl::ConfigDataChangedEvent::getChangedDataSection() const {
  return this->changed_data_section;
}
