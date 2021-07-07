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

#include "account.h"
#include "pjsip_phone_logger.h"
#include <sstream>


doorctrl::Account::Account(const std::shared_ptr<PhoneCallStateObserver>& call_observer, 
                           const std::shared_ptr<PhoneRegistrationStateObserver>& registration_observer) 
  : pj::Account(), call_observer(call_observer), registration_observer(registration_observer) {

}

doorctrl::Account::~Account() {
  this->shutdown();
}

bool doorctrl::Account::hasCall() const {
   return (this->current_call != nullptr);
}

void doorctrl::Account::removeCurrentCall() {
   PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: removing current call object");
   this->current_call.reset(nullptr);
}


void doorctrl::Account::hangupCurrentCall() {
   if(this->current_call != nullptr && this->current_call->isValid()) {
      try { 
         pj::CallOpParam param;
         param.statusCode = PJSIP_SC_DECLINE;
         this->current_call->hangup(param);

         PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: hanging up current call");

         PhoneCallStateEvent event(this->current_call->getCallState(), PHONE_CALL_STATE_FINISHED);
         this->call_observer->handleEvent(event);
      } catch(pj::Error& err) {
         // nothing to do, call already invalid
      }
      this->removeCurrentCall();
   } else {
      PjsipPhoneLogger::getInstance().getOutput().printWarning("Warning: cannot hangup current call, invalid call object");
   }
}


void doorctrl::Account::answerIncomingCall() {
   if(this->current_call != nullptr && this->current_call->isValid()) {
      try {
         PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: pjsip account, answering incoming call");
         pj::CallOpParam param;
         param.statusCode = PJSIP_SC_OK;
         this->current_call->answer(param);
      } catch(pj::Error& err) {
         PjsipPhoneLogger::getInstance().getOutput().printWarning("Warning: error while answering incoming call, removing..." + err.reason);
         this->removeCurrentCall();
      }
   }
}


void doorctrl::Account::dialDtmf(const std::string& code) {
   if(this->current_call != nullptr && this->current_call->isValid()) {
      try {
         this->current_call->dialDtmf(code);
      } catch(pj::Error& err) {
         PjsipPhoneLogger::getInstance().getOutput().printWarning("Warning: error while dialing dtmf, reason " + err.reason);
      }
   }
}


void doorctrl::Account::setUserName(const std::string& name) {
  this->userName = name;
}


const std::string& doorctrl::Account::getUserName() const {
  return this->userName;
}


void doorctrl::Account::onRegStarted(pj::OnRegStartedParam& prm) {
    PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: starting phone registration process");
    this->registration_observer->handleEvent(doorctrl::PhoneRegistrationStateEvent::getInProgressStateEvent()); 
}


void doorctrl::Account::onRegState(pj::OnRegStateParam& prm) {
    pj::AccountInfo ai = getInfo();
    PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: phone registration state has changed " + std::string(ai.regIsActive? "*** Register:" : "*** Unregister:") + " code=" + std::to_string(prm.code));

    if(prm.code == 200) {
      this->registration_observer->handleEvent(doorctrl::PhoneRegistrationStateEvent::getRegisteredStateEvent()); 
    } else if(prm.code == 401) {
      this->registration_observer->handleEvent(doorctrl::PhoneRegistrationStateEvent::getAuthErrorStateEvent()); 
    } else {
      this->registration_observer->handleEvent(doorctrl::PhoneRegistrationStateEvent::getUnregisteredStateEvent()); 
    }
}


void doorctrl::Account::onIncomingCall(pj::OnIncomingCallParam& iprm)
{
   std::stringstream sstream;
   sstream << "Debug: OnIncomingCall has been called, current_call = " << this->current_call.get();
   PjsipPhoneLogger::getInstance().getOutput().printDebug(sstream.str()); 

   if(this->current_call == nullptr) {
      this->current_call = std::make_unique<doorctrl::Call>(this->call_observer, *this, iprm.callId);
      PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: *** Incoming Call *******"); 
      PhoneCallStateEvent event(this->call_state, PHONE_CALL_STATE_RINGING);
      this->call_observer->handleEvent(event); 
   } else if(this->current_call->isValid() != true) {
      this->current_call = std::make_unique<doorctrl::Call>(this->call_observer, *this, iprm.callId);
      PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: *** Incoming Call *******"); 
      PhoneCallStateEvent event(this->call_state, PHONE_CALL_STATE_RINGING);
      this->call_observer->handleEvent(event); 
   }
}
