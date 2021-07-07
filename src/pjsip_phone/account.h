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

#ifndef __DOORCTRL_PJSIP_PHONE_ACCOUNT_H__
#define __DOORCTRL_PJSIP_PHONE_ACCOUNT_H__

#include <pjsua2.hpp>
#include <string>
#include <memory>
#include "call.h"
#include "common/phone_call_state_observer.h"
#include "common/phone_registration_state_observer.h"

namespace doorctrl {

  class Account : public pj::Account {
    public:
      explicit Account(const std::shared_ptr<PhoneCallStateObserver>& call_observer, 
                       const std::shared_ptr<PhoneRegistrationStateObserver>& registration_observer);
      virtual ~Account();
      bool hasCall() const;
      void hangupCurrentCall();
      void answerIncomingCall();
      void dialDtmf(const std::string& code);
      const std::string& getUserName() const;
      void setUserName(const std::string& name);
      void onRegStarted(pj::OnRegStartedParam& prm) override;
      void onRegState(pj::OnRegStateParam& prm) override;
      void onIncomingCall(pj::OnIncomingCallParam& iprm) override;

    private:
      void removeCurrentCall();
      std::unique_ptr<Call> current_call;
      std::shared_ptr<PhoneCallStateObserver> call_observer;
      std::shared_ptr<PhoneRegistrationStateObserver> registration_observer;
      std::string userName = "";
      PhoneCallState call_state = PHONE_CALL_STATE_IDLE;
  };
}

#endif
