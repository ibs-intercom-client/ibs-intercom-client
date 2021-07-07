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

#include "call.h"
#include "pjsip_phone_logger.h"


doorctrl::Call::Call(const std::shared_ptr<PhoneCallStateObserver>& observer, pj::Account& acc, int call_id)
  : pj::Call(acc, call_id), observer(observer) { 

}

doorctrl::Call::~Call() {

}

void doorctrl::Call::onCallState(pj::OnCallStateParam& prm) {
   PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: call state has been changed");
   PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: event type: " + std::string(pjsip_event_str(prm.e.type)));

   pj::CallInfo info = this->getInfo();
   PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: status code = " + std::to_string(info.lastStatusCode));

   if(info.lastStatusCode == PJSIP_SC_REQUEST_TERMINATED) {
     PhoneCallStateEvent event(this->call_state, PHONE_CALL_STATE_TERMINATED);
     this->observer->handleEvent(event);
     this->call_state = PHONE_CALL_STATE_TERMINATED;
     this->enterIdleState();
     this->valid = false;

   } else if(info.lastStatusCode == PJSIP_SC_DECLINE) {
     PhoneCallStateEvent event(this->call_state, PHONE_CALL_STATE_FINISHED);
     this->observer->handleEvent(event);
     this->call_state = PHONE_CALL_STATE_FINISHED;
     this->enterIdleState();
     this->valid = false;

   } else if(info.lastStatusCode == PJSIP_SC_OK) {
     if(prm.e.type == PJSIP_EVENT_TSX_STATE) {
       PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: status: " +  prm.e.body.tsxState.tsx.statusText);
       PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: state: " + std::to_string(prm.e.body.tsxState.tsx.state));
       PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: prev state: " + std::to_string(prm.e.body.tsxState.prevState));

       if(prm.e.body.tsxState.tsx.state == PJSIP_TSX_STATE_TERMINATED ||
          prm.e.body.tsxState.tsx.state == PJSIP_TSX_STATE_DESTROYED) {

          PhoneCallStateEvent event(this->call_state, PHONE_CALL_STATE_TERMINATED);
          this->observer->handleEvent(event);
          this->call_state = PHONE_CALL_STATE_TERMINATED;
          this->enterIdleState();
          this->valid = false;
       } if(prm.e.body.tsxState.tsx.state == PJSIP_TSX_STATE_CONFIRMED) {
          PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: confirmed");
          PhoneCallStateEvent event(this->call_state, PHONE_CALL_STATE_ESTABLISHED);
          this->observer->handleEvent(event);
          this->call_state = PHONE_CALL_STATE_ESTABLISHED;
       } if(prm.e.body.tsxState.tsx.state == PJSIP_TSX_STATE_COMPLETED) {
          PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: completed");
          if(prm.e.body.tsxState.prevState == PJSIP_TSX_STATE_PROCEEDING) {
            PhoneCallStateEvent event(this->call_state, PHONE_CALL_STATE_ESTABLISHED);
            this->observer->handleEvent(event);
            this->call_state = PHONE_CALL_STATE_ESTABLISHED;
            this->valid = true;
          } else if(prm.e.body.tsxState.prevState == PJSIP_TSX_STATE_NULL) {
            PhoneCallStateEvent event(this->call_state, PHONE_CALL_STATE_TERMINATED);
            this->observer->handleEvent(event);
            this->call_state = PHONE_CALL_STATE_TERMINATED;
            this->enterIdleState();
            this->valid = false;
          }
       }
     }  
  }
}


void doorctrl::Call::onCallMediaState(pj::OnCallMediaStateParam& prm) {
   PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: media state changed");

   pj::CallInfo info = this->getInfo();

   //Iterate all the call medias
   for(unsigned i = 0; i < info.media.size(); i++) {
     PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: checking media " + std::to_string(i));
     if(info.media[i].type == PJMEDIA_TYPE_AUDIO && this->getMedia(i) != nullptr) {
       pj::AudioMedia* aud_media = (pj::AudioMedia*)this->getMedia(i);

       //Connect the call audio media to sound device
       pj::AudDevManager& mgr = pj::Endpoint::instance().audDevManager();

       PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: snd is active = " +  std::to_string(mgr.sndIsActive()));

       try {
         PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: connecting AUDIO MEDIA to playback device " +  std::to_string(mgr.getPlaybackDev()));
         PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: connecting AUDIO MEDIA to capture device " +  std::to_string(mgr.getCaptureDev()));
         aud_media->startTransmit(mgr.getPlaybackDevMedia());
         mgr.getCaptureDevMedia().startTransmit(*aud_media);

         PjsipPhoneLogger::getInstance().getOutput().printDebug("Debug: snd is active = " + std::to_string(mgr.sndIsActive()));
       } catch(pj::Error& err) {
         PjsipPhoneLogger::getInstance().getOutput().printWarning("Warning: error while activating sound devices for incoming call, reason = " + err.reason);
       }
     }
   }
}


bool doorctrl::Call::isValid() const {
  return this->valid;
}


void doorctrl::Call::enterIdleState() {
  PhoneCallStateEvent event(this->call_state, PHONE_CALL_STATE_IDLE);
  this->observer->handleEvent(event);
  this->call_state = PHONE_CALL_STATE_IDLE;
}


doorctrl::PhoneCallState doorctrl::Call::getCallState() const {
  return this->call_state;
}
