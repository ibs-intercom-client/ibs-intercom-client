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

#include "echo_account.h"
#include <iostream>

doorctrl::EchoAccount::EchoAccount()
  : pj::Account() {

}

doorctrl::EchoAccount::~EchoAccount() {
  this->shutdown();
}

bool doorctrl::EchoAccount::hasCall() const {
   return (this->current_call != nullptr);
}

void doorctrl::EchoAccount::removeCurrentCall() {
  this->current_call.reset(nullptr);
}


void doorctrl::EchoAccount::hangupCurrentCall() {
   if(this->current_call != nullptr && this->current_call->isValid()) {
         pj::CallOpParam param;
         param.statusCode = PJSIP_SC_NULL;
         this->current_call->hangup(param);

         this->removeCurrentCall();
   } else {
         this->removeCurrentCall();
   }
}


void doorctrl::EchoAccount::makeCall() {
   if(this->current_call == nullptr) {
     this->current_call = std::make_unique<doorctrl::EchoCall>(*this); 

     pj::CallOpParam param(true);

     try {
       this->current_call->makeCall("sip:1001@192.168.200.153", param);
     } catch(pj::Error& err) {
       std::cout << err.info() << std::endl;
     }
   } else {
     std::cout << "call already existing, please hangup first" << std::endl;
   }
}


void doorctrl::EchoAccount::onRegState(pj::OnRegStateParam& prm) {
    pj::AccountInfo ai = getInfo();
    std::cout << "phone registration state has changed " << (ai.regIsActive? "*** Register:" : "*** Unregister:") << " code=" << prm.code << std::endl;

    if(prm.code == 200) {

    } else if(prm.code == 401) {

    } else {

    }
}

