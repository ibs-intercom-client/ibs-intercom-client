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

#include "phone_registration_state_event.h"

static const doorctrl::PhoneRegistrationStateEvent registered_event(doorctrl::PHONE_REGISTRATION_STATE_REGISTERED); 
static const doorctrl::PhoneRegistrationStateEvent unregistered_event(doorctrl::PHONE_REGISTRATION_STATE_UNREGISTERED); 
static const doorctrl::PhoneRegistrationStateEvent in_progress_event(doorctrl::PHONE_REGISTRATION_STATE_IN_PROGRESS); 
static const doorctrl::PhoneRegistrationStateEvent auth_error_event(doorctrl::PHONE_REGISTRATION_STATE_AUTH_ERROR); 

doorctrl::PhoneRegistrationStateEvent::PhoneRegistrationStateEvent(PhoneRegistrationState curr) 
 : curr(curr) {

}


doorctrl::PhoneRegistrationState doorctrl::PhoneRegistrationStateEvent::getCurrentRegistrationState() const {
  return this->curr;
} 


const doorctrl::PhoneRegistrationStateEvent& doorctrl::PhoneRegistrationStateEvent::getRegisteredStateEvent() {
  return registered_event;
}


const doorctrl::PhoneRegistrationStateEvent& doorctrl::PhoneRegistrationStateEvent::getUnregisteredStateEvent() {
  return unregistered_event;
}


const doorctrl::PhoneRegistrationStateEvent& doorctrl::PhoneRegistrationStateEvent::getInProgressStateEvent() {
  return in_progress_event;
}


const doorctrl::PhoneRegistrationStateEvent& doorctrl::PhoneRegistrationStateEvent::getAuthErrorStateEvent() {
  return auth_error_event;
}
