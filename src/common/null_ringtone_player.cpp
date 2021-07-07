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

#include "null_ringtone_player.h"

void doorctrl::NullRingtonePlayer::setup() {
   this->ringing = false;
   this->volume = 100; 
   this->mute = false; 
}

void doorctrl::NullRingtonePlayer::startRinging() {
   this->ringing = true;
}

void doorctrl::NullRingtonePlayer::stopRinging() {
   this->ringing = false;
}

void doorctrl::NullRingtonePlayer::setVolume(uint8_t volume) {
  this->volume = volume; 
}

uint8_t doorctrl::NullRingtonePlayer::getVolume() {
   return this->volume; 
}

void doorctrl::NullRingtonePlayer::setMute(bool mute) {
   this->mute = mute; 
}

bool doorctrl::NullRingtonePlayer::getMute() {
   return this->mute; 
}
