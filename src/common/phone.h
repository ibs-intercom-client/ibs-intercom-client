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

#ifndef __DOORCTRL_COMMON_PHONE_H__
#define __DOORCTRL_COMMON_PHONE_H__

#include <string>
#include "credentials.h"
#include "observed_subject.h"
#include "phone_call_state_observer.h"
#include "phone_registration_state_observer.h"

namespace doorctrl {

class Phone : public ObservedSubject<PhoneCallStateObserver>, 
              public ObservedSubject<PhoneRegistrationStateObserver> {
  public:
    virtual ~Phone() = default;
    virtual void setup() = 0;
    virtual void cleanup() = 0;
    virtual void registerPhone() = 0;
    virtual void answer() = 0;
    virtual void hangup() = 0;
    virtual void muteMicrophone(bool mute) = 0;
    virtual void setSpeakerVolume(uint8_t volume) = 0;
    virtual uint8_t getSpeakerVolume() const = 0;
    virtual bool hasEstablishedCall() = 0;
    virtual bool hasIncomingCall() = 0;
    virtual bool isRegistering() = 0;
    virtual const std::string &getRegisteredUserName() const = 0;
  };
}

#endif
