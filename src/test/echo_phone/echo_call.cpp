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

#include "echo_call.h"
#include <iostream>


doorctrl::EchoCall::EchoCall(pj::Account& acc, int call_id)
  : pj::Call(acc, call_id) {
//
  this->player.createPlayer("/usr/share/sounds/alsa/Front_Center.wav");
//  this->player.createPlayer("./tagesthemen.wav");
//  this->player.createPlayer("./sine_500.wav");
//  this->player.createPlayer("./ticks.wav");
  this->recorder.createRecorder("./nix.wav");
}

void doorctrl::EchoCall::onCallState(pj::OnCallStateParam& prm) {
   std::cout << "call state has been changed" << std::endl;
   std::cout << "event type: " << pjsip_event_str(prm.e.type) << std::endl;

   pj::CallInfo info = this->getInfo();

   if(info.lastStatusCode == PJSIP_SC_REQUEST_TERMINATED) {
     this->valid = false;
   } else if(info.lastStatusCode == PJSIP_SC_DECLINE) {
     this->valid = false;
   } else if(info.lastStatusCode == PJSIP_SC_OK) {
     if(prm.e.type == PJSIP_EVENT_TSX_STATE) {

       if(prm.e.body.tsxState.tsx.state == PJSIP_TSX_STATE_TERMINATED ||
          prm.e.body.tsxState.tsx.state == PJSIP_TSX_STATE_DESTROYED) {
          this->valid = false;
          if(this->hasMedia()) {
             this->recorder.stopTransmit(this->getAudioMedia(-1));
          }
       } if(prm.e.body.tsxState.tsx.state == PJSIP_TSX_STATE_CONFIRMED) {
       } if(prm.e.body.tsxState.tsx.state == PJSIP_TSX_STATE_COMPLETED) {
          if(prm.e.body.tsxState.prevState == PJSIP_TSX_STATE_PROCEEDING) {
            this->valid = true;
          } else if(prm.e.body.tsxState.prevState == PJSIP_TSX_STATE_NULL) {
            this->valid = false;
          }
       }
     }  
  }
}


void doorctrl::EchoCall::onCallMediaState(pj::OnCallMediaStateParam& prm) {
   std::cout << "media state changed" << std::endl;

   pj::CallInfo info = this->getInfo();

   //Iterate all the call medias
   for(unsigned i = 0; i < info.media.size(); i++) {
     std::cout << "=============== checking media ================" << i;
     if(info.media[i].type == PJMEDIA_TYPE_AUDIO && this->getMedia(i) != nullptr) {

//       pj::AudioMedia* aud_media = (pj::AudioMedia*)this->getMedia(i);
//       pj::AudioMedia* aud_media = pj::AudioMedia::typecastFromMedia(this->getMedia(i));
       pj::AudioMedia aud_media = this->getAudioMedia(-1);

       try {

         player.startTransmit(aud_media);
         aud_media.startTransmit(recorder);
         player.startTransmit(recorder);
         
       } catch(pj::Error& err) {
         std::cout << "error while activating sound devices for outgoing call, reason = " << err.reason << std::endl;
       }
     }

   }

}


bool doorctrl::EchoCall::isValid() const {
  return this->valid;
}

