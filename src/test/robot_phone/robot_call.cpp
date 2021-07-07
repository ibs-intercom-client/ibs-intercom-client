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

#include "robot_call.h"
#include <iostream>


doorctrl::RobotCall::RobotCall(pj::Account& acc, int call_id)
  : pj::Call(acc, call_id) {

}

void doorctrl::RobotCall::onCallState(pj::OnCallStateParam& prm) {
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


void doorctrl::RobotCall::onCallMediaState(pj::OnCallMediaStateParam& prm) {
   std::cout << "media state changed" << std::endl;

   pj::CallInfo info = this->getInfo();

   //Iterate all the call medias
   for(unsigned i = 0; i < info.media.size(); i++) {
     std::cout << "checking media " << i;
     if(info.media[i].type == PJMEDIA_TYPE_AUDIO && this->getMedia(i) != nullptr) {
       pj::AudioMedia* aud_media = (pj::AudioMedia*)this->getMedia(i);

       //Connect the call audio media to sound device
       pj::AudDevManager& mgr = pj::Endpoint::instance().audDevManager();

       try {
/*
         aud_media->startTransmit(mgr.getPlaybackDevMedia());
         mgr.getCaptureDevMedia().startTransmit(*aud_media);
*/
       } catch(pj::Error& err) {
         std::cout << "error while activating sound devices for outgoing call, reason = " << err.reason << std::endl;
       }
     }

   }

}


bool doorctrl::RobotCall::isValid() const {
  return this->valid;
}

