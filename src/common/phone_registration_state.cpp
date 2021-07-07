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

#include "phone_registration_state.h" 

std::string doorctrl::PhoneRegistrationStateToString(doorctrl::PhoneRegistrationState state) {
  switch(state) {
    case PHONE_REGISTRATION_STATE_UNREGISTERED:
      return "PHONE_REGISTRATION_STATE_UNREGISTERED";
    case PHONE_REGISTRATION_STATE_IN_PROGRESS:
      return "PHONE_REGISTRATION_STATE_IN_PROGRESS";
    case PHONE_REGISTRATION_STATE_REGISTERED:  
      return "PHONE_REGISTRATION_STATE_REGISTERED";
    case PHONE_REGISTRATION_STATE_AUTH_ERROR:  
      return "PHONE_REGISTRATION_STATE_AUTH_ERROR";
    default:
      return "PHONE_REGISTRATION_STATE_UNKNOWN";
  }
}
