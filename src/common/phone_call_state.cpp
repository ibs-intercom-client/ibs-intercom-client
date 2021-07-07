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

#include "phone_call_state.h" 

std::string doorctrl::phoneCallStateToString(doorctrl::PhoneCallState state) {
  switch(state) {
    case PHONE_CALL_STATE_IDLE:
      return "PHONE_CALL_STATE_IDLE";
    case PHONE_CALL_STATE_RINGING:
      return "PHONE_CALL_STATE_RINGING";
    case PHONE_CALL_STATE_ESTABLISHED:  
      return "PHONE_CALL_STATE_ESTABLISHED";
    case PHONE_CALL_STATE_TERMINATED:  
      return "PHONE_CALL_STATE_TERMINATED";
    case PHONE_CALL_STATE_FINISHED:  
      return "PHONE_CALL_STATE_FINISHED";
    default:
      return "PHONE_CALL_STATE_UNKNOWN";
  }
}
