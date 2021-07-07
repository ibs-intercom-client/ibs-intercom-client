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

#ifndef __DOORCTRL_CONFIG_CONFIG_DATA_CHANGED_EVENT_H__
#define __DOORCTRL_CONFIG_CONFIG_DATA_CHANGED_EVENT_H__

#include <string>
#include "config_data_event.h"

namespace doorctrl {

  class ConfigDataChangedEvent : public ConfigDataEvent {
    public:
      static const std::string SIP_SECTION_NAME;
      static const std::string RINGTONE_SECTION_NAME;
      static const std::string VIDEO_SECTION_NAME;
      static const std::string LOGGING_SECTION_NAME;
      static const std::string PHONE_SECTION_NAME;
      static const std::string PUSH_BUTTON_SECTION_NAME;
      static const std::string HOME_BUTTON_SECTION_NAME;
      static const std::string APPLICATION_SECTION_NAME;
      static const std::string HEALTH_SECTION_NAME;
      static const std::string HOMEGEAR_RPC_SECTION_NAME;
      static const std::string UI_SECTION_NAME;

      explicit ConfigDataChangedEvent(const std::string& data_section); 
      const std::string& getChangedDataSection() const; 

      static const ConfigDataChangedEvent& getSipConfigChangedEvent();
      static const ConfigDataChangedEvent& getRingtoneConfigChangedEvent();
      static const ConfigDataChangedEvent& getVideoConfigChangedEvent();
      static const ConfigDataChangedEvent& getLoggingConfigChangedEvent();
      static const ConfigDataChangedEvent& getPhoneConfigChangedEvent();
      static const ConfigDataChangedEvent& getPushButtonConfigChangedEvent();
      static const ConfigDataChangedEvent& getHomeButtonConfigChangedEvent();
      static const ConfigDataChangedEvent& getApplicationConfigChangedEvent();
      static const ConfigDataChangedEvent& getHealthConfigChangedEvent();
      static const ConfigDataChangedEvent& getHomegearRpcConfigChangedEvent();
      static const ConfigDataChangedEvent& getUiConfigChangedEvent();

    private:
      std::string changed_data_section = "";
  }; 
}

#endif
