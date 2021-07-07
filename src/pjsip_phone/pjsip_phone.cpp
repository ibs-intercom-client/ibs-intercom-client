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

#include "pjsip_phone.h"

#include <memory>
#include "pjsip_phone_logger.h"
#include "common/simple_observer_manager.cpp"
#include "common/phone_setup_exception.h"

static std::string defaultUserName = "no user";


doorctrl::PjsipPhone::PjsipPhone(std::shared_ptr<ConfigData>& config_data)
  : config_data(config_data), call_established(false)  {

}

doorctrl::PjsipPhone::~PjsipPhone() {
  this->config_data->removeObserver(shared_from_this());
}

void doorctrl::PjsipPhone::setup() {
 
  this->config_data->addObserver(shared_from_this());

  try {
     std::lock_guard<std::mutex> guard(phone_mutex);
     endpoint.libCreate();

     pj::EpConfig ep_cfg;
     ep_cfg.logConfig.level = this->config_data->getLoggingConfig().console_loglevel;
//     ep_cfg.logConfig.level = 6; 
//     ep_cfg.medConfig.channelCount = 2;
     ep_cfg.medConfig.channelCount = 1;
//     ep_cfg.medConfig.audioFramePtime = 32;
//     ep_cfg.medConfig.audioFramePtime = 64;
     ep_cfg.medConfig.audioFramePtime = 128;

     if(this->config_data->getSipConfig().ec_enabled) {
        ep_cfg.medConfig.noVad = true;
        ep_cfg.medConfig.ecOptions = PJMEDIA_ECHO_SPEEX;

        if(this->config_data->getSipConfig().ec_noise_suppressor_enabled) {
           ep_cfg.medConfig.ecOptions |= PJMEDIA_ECHO_USE_NOISE_SUPPRESSOR;
        }


        switch(this->config_data->getSipConfig().ec_aggressiveness) {
           case 1:
             ep_cfg.medConfig.ecOptions |= PJMEDIA_ECHO_AGGRESSIVENESS_CONSERVATIVE;
             break;
           case 2:
             ep_cfg.medConfig.ecOptions |= PJMEDIA_ECHO_AGGRESSIVENESS_MODERATE;
             break;
           case 3:
             ep_cfg.medConfig.ecOptions |= PJMEDIA_ECHO_AGGRESSIVENESS_AGGRESSIVE;
             break;
           default:
             ep_cfg.medConfig.ecOptions |= PJMEDIA_ECHO_AGGRESSIVENESS_DEFAULT;
        } 

        ep_cfg.medConfig.ecTailLen = this->config_data->getSipConfig().ec_tail_len;
     }

     endpoint.libInit(ep_cfg);
  } catch(pj::Error& err) {
     PjsipPhoneLogger::getInstance().getOutput().printError("Error: failed to setup phone, error while initializing pjsip lib");
     throw PhoneSetupException("error while initializing phone");
  }

  for(unsigned int i = 0; i < endpoint.audDevManager().getDevCount(); i++) {
    PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: " + std::to_string(i) + "  " + endpoint.audDevManager().getDevInfo(i).name); 
  }

  pj::TransportConfig tp_cfg;
  PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: audio start UDP port " + std::to_string(this->config_data->getSipConfig().audio_port_start));
  tp_cfg.port = this->config_data->getSipConfig().udp_port;

  PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: audio bind address " + this->config_data->getSipConfig().audio_bind_address);

  std::string audio_bind_address = this->config_data->getSipConfig().audio_bind_address;

  if(!BaseLib::Net::isIp(audio_bind_address)) {
     // Use interface name instead of IP e.g. eth0.60, translate interface name to IP
     audio_bind_address  = BaseLib::Net::getMyIpAddress(audio_bind_address);
  }

  PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: audio bind address " + audio_bind_address);

  tp_cfg.publicAddress = audio_bind_address;
  tp_cfg.boundAddress =  audio_bind_address;

  try {
    endpoint.transportCreate(PJSIP_TRANSPORT_UDP, tp_cfg);
  } catch (pj::Error &err) {
     PjsipPhoneLogger::getInstance().getOutput().printError("Error: failed to setup phone, error while creating phone UDP transport, port = " + std::to_string(this->config_data->getSipConfig().udp_port));
     throw PhoneSetupException("error while creating phone udp transport");
  }

//  endpoint.audDevManager().setInputVolume(this->config_data->getPhoneConfig().mic_level);

  endpoint.libStart();

  this->setupVolume(this->config_data->getPhoneConfig(), endpoint);

  {
    std::lock_guard<std::mutex> guard(phone_mutex);
    this->pjsip_initialized = true;
  }

  this->config_data->addObserver(shared_from_this());

  PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: phone setup finished successfully");
}

void doorctrl::PjsipPhone::cleanup() {
  
}


void doorctrl::PjsipPhone::registerPhone() {
  pj::AccountConfig acc_cfg;
  SipConfig sip_config = this->config_data->getSipConfig();

  if(sip_config.registrar.empty()) {
    PjsipPhoneLogger::getInstance().getOutput().printError("Error: failed to setup phone, sip registrar uri must not be empty");
    throw PhoneSetupException("sip registrar uri must not be empty, cannot setup phone");
  }

  if(sip_config.credentials.username.empty()) {
    PjsipPhoneLogger::getInstance().getOutput().printError("Error: failed to setup phone, sip username must not be empty");
    throw PhoneSetupException("sip username must not be empty, cannot setup phone");
  }

  std::string uri = "sip:" + sip_config.credentials.username + "@" + sip_config.registrar + ":" + std::to_string(sip_config.udp_port);

  PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: creating uri to register phone at sip registrar server, uri = '" + uri + "'");

  acc_cfg.idUri = uri;

  acc_cfg.regConfig.registrarUri = "sip:" + sip_config.registrar + ":" + std::to_string(sip_config.udp_port);
  acc_cfg.regConfig.timeoutSec = sip_config.reg_interval; 
  pj::AuthCredInfo info("digest", "*", sip_config.credentials.username, 0, sip_config.credentials.password);
  acc_cfg.sipConfig.authCreds.push_back( info );
  acc_cfg.mediaConfig.transportConfig.port = this->config_data->getSipConfig().audio_port_start;
  acc_cfg.mediaConfig.transportConfig.portRange = this->config_data->getSipConfig().audio_port_range;

  std::string audio_bind_address = this->config_data->getSipConfig().audio_bind_address;
  if(!BaseLib::Net::isIp(audio_bind_address)) {
     // Use interface name instead of IP e.g. eth0.60, translate interface name to IP
     audio_bind_address = BaseLib::Net::getMyIpAddress(audio_bind_address);
  }
  PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: audio_bind_address = " + audio_bind_address);
  acc_cfg.mediaConfig.transportConfig.boundAddress = audio_bind_address;

  this->acc = std::make_unique<Account>(shared_from_this(), shared_from_this());
  this->acc->setUserName(sip_config.credentials.username);

  PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: creating pjsip account");
  this->acc->create(acc_cfg);
}


void doorctrl::PjsipPhone::answer() {
   if(this->acc != nullptr) {
      this->acc->answerIncomingCall();
   } 
}


void doorctrl::PjsipPhone::hangup() {
   if(this->acc != nullptr) {
      this->acc->hangupCurrentCall();
   }
}


void doorctrl::PjsipPhone::buzz() {
   if(this->acc != nullptr) {
      this->acc->dialDtmf("00*");
   }
}


void doorctrl::PjsipPhone::setMicrophoneLevel(uint8_t level) {
    if(this->mic_level != level) {
       PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: set microphone, level = " + std::to_string(level));
       this->mic_level = level;
       auto pjsip_mic_level = (float)((float)(this->mic_level) / 100.0);
       endpoint.audDevManager().getCaptureDevMedia().adjustTxLevel(pjsip_mic_level);
    }
}


void doorctrl::PjsipPhone::muteMicrophone(bool mute) {
  if(mute) {
    PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: muting microphone, level = 0");
    endpoint.audDevManager().getCaptureDevMedia().adjustTxLevel(0.0);
  } else {
    auto pjsip_mic_level = (float)((float)(this->mic_level) / 100.0);
    PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: unmuting microphone, level = " + std::to_string((uint16_t)this->mic_level));
    endpoint.audDevManager().getCaptureDevMedia().adjustTxLevel(pjsip_mic_level);
  }
}


void doorctrl::PjsipPhone::setSpeakerVolume(uint8_t volume) {

   if(this->speaker_volume != volume) {
      auto pjsip_volume = (float)((float)(volume) / 100.0);
      PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: setting speaker volume, value = " + std::to_string((uint16_t)volume));
      endpoint.audDevManager().getPlaybackDevMedia().adjustRxLevel(pjsip_volume);
      this->speaker_volume = volume;
   }
}


uint8_t doorctrl::PjsipPhone::getSpeakerVolume() const {
   return this->speaker_volume;
}


bool doorctrl::PjsipPhone::hasEstablishedCall() {
   std::lock_guard<std::mutex> guard(this->phone_mutex); 
   PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: has etablished call = " + std::to_string((this->acc->hasCall() && this->call_established)));
   return (this->acc->hasCall() && this->call_established);
}


bool doorctrl::PjsipPhone::hasIncomingCall() {

   std::lock_guard<std::mutex> guard(this->phone_mutex); 
   PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: has incoming call = " + std::to_string((this->acc->hasCall() && !this->call_established)));
   return  (this->acc->hasCall() && !this->call_established);
}


bool doorctrl::PjsipPhone::isRegistering() {

   std::lock_guard<std::mutex> guard(this->phone_mutex); 
   PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: is registering = " + std::to_string(this->phone_registering));
   return this->phone_registering; 
}


const std::string& doorctrl::PjsipPhone::getRegisteredUserName() const {
  if(!this->config_data->getSipConfig().display_name.empty()) {
     return this->config_data->getSipConfig().display_name;
  } else if(this->acc != nullptr) {
     return this->acc->getUserName();
  } else {
     return defaultUserName;
  } 
}


void doorctrl::PjsipPhone::addObserver(const std::shared_ptr<doorctrl::PhoneCallStateObserver>& observer) {
  return this->call_observer_manager.addObserver(observer);
}

void doorctrl::PjsipPhone::removeObserver(const std::shared_ptr<doorctrl::PhoneCallStateObserver>& observer) {
  this->call_observer_manager.removeObserver(observer);
}

void doorctrl::PjsipPhone::addObserver(const std::shared_ptr<doorctrl::PhoneRegistrationStateObserver>& observer) {
  return this->registration_observer_manager.addObserver(observer);
}

void doorctrl::PjsipPhone::removeObserver(const std::shared_ptr<doorctrl::PhoneRegistrationStateObserver>& observer) {
  this->registration_observer_manager.removeObserver(observer);
}


bool doorctrl::PjsipPhone::isInitialized() {
   std::lock_guard<std::mutex> guard(this->phone_mutex); 

   return this->pjsip_initialized;
}


void doorctrl::PjsipPhone::handleEvent(const ConfigDataChangedEvent& event) {

  if(!this->isInitialized()) {
     return;
  }

  if(!this->endpoint.libIsThreadRegistered()) {
     this->endpoint.libRegisterThread("event thread");
  }

  if(event.getChangedDataSection() == doorctrl::ConfigDataChangedEvent::PHONE_SECTION_NAME) {

     /*
      * this method is executed by many different threads. pjsip does not
      * allow calls to pjsib lib without registering those threads.
      * libInit and libStart must have been called before otherwise
      * an assertion will fail which exits this system process.
      */  
     if(!this->endpoint.libIsThreadRegistered()) {
        this->endpoint.libRegisterThread("event thread");
     }
     this->setSpeakerVolume(config_data->getPhoneConfig().volume);
     this->setMicrophoneLevel(config_data->getPhoneConfig().mic_level);
  }
}


void doorctrl::PjsipPhone::handleEvent(const PhoneCallStateEvent& event) {
   std::lock_guard<std::mutex> guard(this->phone_mutex); 
 
   switch(event.getCurrentCallState()) {
     case PHONE_CALL_STATE_ESTABLISHED:
       this->call_established = true;
       break;
     default:
       this->call_established = false;
   }

   PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: phone call state has changed, sending event to all phone observers");
   this->call_observer_manager.fireEvent(event);
}


void doorctrl::PjsipPhone::handleEvent(const PhoneRegistrationStateEvent& event) {

   std::lock_guard<std::mutex> guard(this->phone_mutex); 

   bool last_phone_registering = this->phone_registering;

   if(event.getCurrentRegistrationState() == doorctrl::PHONE_REGISTRATION_STATE_IN_PROGRESS) {
      this->phone_registering = true;
   } else {
      this->phone_registering = false;
   }
   

   /*
    * The registration event also comes in case of re-registering
    * which is done after a configurable amount of time.
    * Sending this event to observers outside the phone will
    * cause irritations, e.g. switch the UI in neutral view.
    * Thus, send just registration changes, do not confirm the current state.
    */ 
   if(last_phone_registering != this->phone_registering) {

     PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: phone registration state has changed, sending event to all phone observers");

     this->registration_observer_manager.fireEvent(event);
   }
}


void doorctrl::PjsipPhone::setupVolume(const PhoneConfig& phone_config, pj::Endpoint& endpoint) {
  try {
    auto configured_volume = (float)((float)(phone_config.volume) / 100.0);
    auto configured_mic_level = (float)((float)(phone_config.mic_level) / 100.0);

    if(configured_volume < 0.0) {
      PjsipPhoneLogger::getInstance().getOutput().printWarning("Warning: invalid value, speaker volume less than 0, value auto corrected to 0");
      configured_volume = 0.0;
    }

    if(configured_volume > 1.0) {
      PjsipPhoneLogger::getInstance().getOutput().printWarning("Warning: invalid value, speaker volume greater than 100, value auto corrected to 100");
      configured_volume = 1.0;
    }

    if(configured_mic_level < 0.0) {
      PjsipPhoneLogger::getInstance().getOutput().printWarning("Warning: invalid value, mic level less than 0, value auto corrected to 0");
      configured_mic_level = 0.0;
    }

    if(configured_mic_level > 1.0) {
      PjsipPhoneLogger::getInstance().getOutput().printWarning("Warning: invalid value, mic level greater than 100, value auto corrected to 100");
      configured_mic_level = 1.0;
    }

    PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: setup pjsip speaker volume, value = " +  std::to_string(configured_volume));
    PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: setup pjsip mic level, value = " + std::to_string(configured_mic_level));

    this->speaker_volume = configured_volume * 100;
    this->mic_level = configured_mic_level * 100;

    endpoint.audDevManager().getPlaybackDevMedia().adjustRxLevel(configured_volume);

    if(phone_config.mute_mic) {
       this->muteMicrophone(true);
    } else {
       endpoint.audDevManager().getCaptureDevMedia().adjustTxLevel(configured_mic_level);
    }

  } catch (pj::Error &err) {
    PjsipPhoneLogger::getInstance().getOutput().printError("Error: failed to setup playback media volume");
  }
}
