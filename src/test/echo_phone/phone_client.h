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

#ifndef __DOORCTRL_PHONE_CLIENT_H__
#define __DOORCTRL_PHONE_CLIENT_H__

#include <pjsua2.hpp>
#include <mutex>
#include "echo_phone_options.h"
#include "echo_account.h"

namespace doorctrl {

   class PhoneClient {
     public:
       ~PhoneClient(); 
       void setup(const EchoPhoneOptions& options);
       void cleanup();
       void registerPhone();
       void makeCall();
       void hangupCall();

     private:
       std::string registrar = ""; 
       std::string user = ""; 
       std::string passwd = ""; 
       pj::Endpoint endpoint;
       std::unique_ptr<EchoAccount> acc;
       std::mutex phone_mutex;
   }; 
}

#endif
