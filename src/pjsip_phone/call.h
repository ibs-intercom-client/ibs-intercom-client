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

#ifndef __DOORCTRL_PJSIP_PHONE_CALL_H__
#define __DOORCTRL_PJSIP_PHONE_CALL_H__

#include <pjsua2.hpp>
#include <memory>
#include "common/phone_call_state_observer.h"

namespace doorctrl {

  class Call : public pj::Call {
    public:
      explicit Call(const std::shared_ptr<PhoneCallStateObserver>& observer, pj::Account& acc, int call_id = PJSUA_INVALID_ID);
      virtual ~Call();
      bool isValid() const;
      void onCallState(pj::OnCallStateParam& prm);
      void onCallMediaState(pj::OnCallMediaStateParam& prm);
      void playDTMFDigits(const std::string& digits);
      PhoneCallState getCallState() const;

    private:
      void setupTonegen();
      void cleanupTonegen();
      void enterIdleState();

      std::shared_ptr<PhoneCallStateObserver> observer;
      bool valid = true;
      pj_pool_t* pool = nullptr; 
      pjmedia_port* tonegen_port = nullptr;
      pjsua_conf_port_id tonegen_slot;
      pj::AudioMediaPlayer* player = nullptr;
      PhoneCallState call_state = PHONE_CALL_STATE_IDLE;
  };
}

#endif
