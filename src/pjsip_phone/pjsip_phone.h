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

#ifndef __DOORCTL_PJSIP_PHONE_PJSIP_PHONE_H__
#define __DOORCTL_PJSIP_PHONE_PJSIP_PHONE_H__

#include <pjsua2.hpp>
#include <mutex>
#include <memory>
#include "common/phone.h"
#include "common/door_opener.h"
#include "common/observed_subject.h"
#include "common/simple_observer_manager.h"
#include "common/phone_call_state_event.h"
#include "config/config_data_changed_observer.h"
#include "config/config_data.h"
#include "account.h"

namespace doorctrl {

  class PjsipPhone : public Phone, 
                     public DoorOpener, 
                     public PhoneCallStateObserver, 
                     public PhoneRegistrationStateObserver, 
                     public ConfigDataChangedObserver,
                     public std::enable_shared_from_this<PjsipPhone> {
    public:
      explicit PjsipPhone(std::shared_ptr<ConfigData>& config_data);
      ~PjsipPhone();
      void setup() override;
      void cleanup() override;
      void registerPhone() override;
      void answer() override;
      void hangup() override;
      void buzz() override;
      void muteMicrophone(bool mute) override;
      void setMicrophoneLevel(uint8_t volume);
      void setSpeakerVolume(uint8_t volume) override;
      uint8_t getSpeakerVolume() const override;
      bool hasEstablishedCall() override;
      bool hasIncomingCall() override;
      bool isRegistering() override;
      const std::string& getRegisteredUserName() const override;
      void addObserver(const std::shared_ptr<PhoneCallStateObserver>& observer) override;
      void removeObserver(const std::shared_ptr<PhoneCallStateObserver>& observer) override;
      void addObserver(const std::shared_ptr<PhoneRegistrationStateObserver>& observer) override;
      void removeObserver(const std::shared_ptr<PhoneRegistrationStateObserver>& observer) override;
      void handleEvent(const ConfigDataChangedEvent& event) override;
      void handleEvent(const PhoneCallStateEvent& event) override;
      void handleEvent(const PhoneRegistrationStateEvent& event) override;
      bool isInitialized();

    private:
      void setupVolume(const PhoneConfig& phone_config, pj::Endpoint& endpoint);

      pj::Endpoint endpoint;  
      std::unique_ptr<Account> acc;
      SimpleObserverManager<PhoneCallStateObserver, PhoneCallStateEvent> call_observer_manager;
      SimpleObserverManager<PhoneRegistrationStateObserver, PhoneRegistrationStateEvent> registration_observer_manager;
      std::shared_ptr<ConfigData> config_data;
      bool call_established = false;
      bool phone_registering = false;
      uint8_t mic_level = 80;
      uint8_t speaker_volume = 80;
      std::mutex phone_mutex;
      bool pjsip_initialized = false;
  };
}
#endif
