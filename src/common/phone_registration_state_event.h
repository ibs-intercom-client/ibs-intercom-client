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

#ifndef __DOORCTRL_COMMON_PHONE_REGISTRATION_STATE_EVENT_H__
#define __DOORCTRL_COMMON_PHONE_REGISTRATION_STATE_EVENT_H__

#include "phone_event.h"
#include "phone_registration_state.h"

namespace doorctrl {
  
  class PhoneRegistrationStateEvent : public PhoneEvent {
    public:
      explicit PhoneRegistrationStateEvent(PhoneRegistrationState curr); 
      PhoneRegistrationState getCurrentRegistrationState() const;

      /*
       * factory methods to get predefined event objects to avoid
       * unnecassary object creation
       */
      static const PhoneRegistrationStateEvent& getRegisteredStateEvent();
      static const PhoneRegistrationStateEvent& getUnregisteredStateEvent();
      static const PhoneRegistrationStateEvent& getInProgressStateEvent();
      static const PhoneRegistrationStateEvent& getAuthErrorStateEvent();

    private:
      PhoneRegistrationState curr = PHONE_REGISTRATION_STATE_UNDEFINED;         
  }; 
}

#endif
