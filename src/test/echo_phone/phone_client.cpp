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

#include "phone_client.h"
#include <iostream>

doorctrl::PhoneClient::~PhoneClient() {
  if(this->acc != nullptr) {
    this->acc->hangupCurrentCall();
  }
}

void doorctrl::PhoneClient::setup(const EchoPhoneOptions& options) {

  try {
     endpoint.libCreate();

     pj::EpConfig ep_cfg;
     ep_cfg.logConfig.level = 5;
     ep_cfg.medConfig.channelCount = 2;
     ep_cfg.medConfig.audioFramePtime = 32;

     endpoint.libInit(ep_cfg);
  } catch(pj::Error& err) {
     std::cout << "failed to setup phone client, error while initializing pjsip lib" << std::endl;
     exit(1);
  }

  pj::TransportConfig tp_cfg;
  tp_cfg.port = options.port;

  try {
    endpoint.transportCreate(PJSIP_TRANSPORT_UDP, tp_cfg);
  } catch (pj::Error &err) {
    std::cout << "failed to setup phone client, error while creating phone UDP transport, port = " << options.port << std::endl;
    exit(1);
  }

  endpoint.libStart();

  this->registrar = options.registrar;
  this->user = options.user;
  this->passwd = options.passwd;

}


void doorctrl::PhoneClient::cleanup() {


}


void doorctrl::PhoneClient::registerPhone() {
  pj::AccountConfig acc_cfg;

  if(this->registrar.empty()) {
    std::cout << "failed to setup phone client, sip registrar must not be empty" << std::endl;
    exit(1);
  }

  if(this->user.empty()) {
    std::cout << "failed to setup phone client, sip username must not be empty << std::endl";
    exit(1);
  }

  std::string uri = "sip:" + this->user + "@" + this->registrar;

  acc_cfg.idUri = uri;

  acc_cfg.regConfig.registrarUri = "sip:" + this->registrar;
  pj::AuthCredInfo info("digest", "*", this->user, 0, this->passwd);
  acc_cfg.sipConfig.authCreds.push_back( info );

  this->acc = std::make_unique<doorctrl::EchoAccount>();

  this->acc->create(acc_cfg);
}


void doorctrl::PhoneClient::makeCall() {
   if(this->acc != nullptr) {
     this->acc->makeCall();
   }
}


void doorctrl::PhoneClient::hangupCall() {
   if(this->acc != nullptr) {
     this->acc->hangupCurrentCall();
   }
}
