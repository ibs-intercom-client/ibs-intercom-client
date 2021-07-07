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

#ifndef __DOORCTRL_ECHO_CALL_H__
#define __DOORCTRL_ECHO_CALL_H__

#include <pjsua2.hpp>
#include <memory>

namespace doorctrl {

  class EchoCall : public pj::Call {
    public:
      explicit EchoCall(pj::Account& acc, int call_id = PJSUA_INVALID_ID);
      virtual ~EchoCall() = default;
      bool isValid() const;
      void onCallState(pj::OnCallStateParam& prm);
      void onCallMediaState(pj::OnCallMediaStateParam& prm);

    private:
      pj_pool_t* pool = nullptr; 
      pjmedia_port* tonegen_port = nullptr;
      pjsua_conf_port_id tonegen_slot;
      pj::AudioMediaPlayer player;
      pj::AudioMediaRecorder recorder;
      bool valid = false;
  };
}

#endif
