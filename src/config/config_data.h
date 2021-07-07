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

#ifndef __DOORCTRL_CONFIG_CONFIG_DATA_H__
#define __DOORCTRL_CONFIG_CONFIG_DATA_H__

#include <list>
#include "sip_config.h"
#include "ringtone_config.h"
#include "ui_config.h"
#include "video_config.h"
#include "logging_config.h"
#include "phone_config.h"
#include "push_button_config.h"
#include "home_button_config.h"
#include "application_config.h"
#include "health_config.h"
#include "homegear_rpc_config.h"
#include "common/observed_subject.h"
#include "common/simple_observer_manager.h"
#include "config_data_changed_observer.h"
#include "config_data_changed_event.h"
#include "config_data_persistence.h"

namespace doorctrl {

  class ConfigData : public ObservedSubject<ConfigDataChangedObserver>, 
                     public std::enable_shared_from_this<ConfigData> {
    public:
      const SipConfig& getSipConfig() const;
      const RingtoneConfig& getRingtoneConfig() const;
      const VideoConfig& getVideoConfig() const;
      const LoggingConfig& getLoggingConfig() const;
      const PhoneConfig& getPhoneConfig() const;
      const PushButtonConfig& getPushButtonConfig() const;
      const HomeButtonConfig& getHomeButtonConfig() const;
      const ApplicationConfig& getApplicationConfig() const;
      const HealthConfig& getHealthConfig() const;
      const HomegearRpcConfig& getHomegearRpcConfig() const;
      const UiConfig& getUiConfig() const;
      void updateSipConfig(const SipConfig& modified_sip_config);
      void updateRingtoneConfig(const RingtoneConfig& modified_ringtone_config);
      void updateVideoConfig(const VideoConfig& modified_video_config);
      void updateLoggingConfig(const LoggingConfig& modified_logging_config);
      void updatePhoneConfig(const PhoneConfig& modified_phone_config);
      void updatePushButtonConfig(const PushButtonConfig& modified_push_button_config);
      void updateHomeButtonConfig(const HomeButtonConfig& modified_home_button_config);
      void updateApplicationConfig(const ApplicationConfig& modified_application_config);
      void updateHealthConfig(const HealthConfig& modified_health_config);
      void updateHomegearRpcConfig(const HomegearRpcConfig& modified_homegear_rpc_config);
      void updateUiConfig(const UiConfig& modified_ui_config);

      void addObserver(const std::shared_ptr<ConfigDataChangedObserver>& observer) override;
      void removeObserver(const std::shared_ptr<ConfigDataChangedObserver>& observer) override;
      void save();
      void addConfigDataPersistence(const std::shared_ptr<ConfigDataPersistence>& persistence);

    private:
      SipConfig sip_config;
      RingtoneConfig ringtone_config;
      VideoConfig video_config;
      LoggingConfig logging_config;
      PhoneConfig phone_config;
      PushButtonConfig push_button_config;
      HomeButtonConfig home_button_config;
      ApplicationConfig application_config;
      HealthConfig health_config;
      HomegearRpcConfig homegear_rpc_config;
      UiConfig ui_config;
      SimpleObserverManager<ConfigDataChangedObserver, ConfigDataChangedEvent> observer_manager;
      std::list<std::shared_ptr<ConfigDataPersistence> > persistence_list;   
  };
}

#endif
