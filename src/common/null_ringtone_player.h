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

#ifndef __DOORCTRL_COMMON_NULL_RINGTONE_PLAYER_H__
#define __DOORCTRL_COMMON_NULL_RINGTONE_PLAYER_H__

#include "ringtone_player.h" 

namespace doorctrl {

  class NullRingtonePlayer : public RingtonePlayer {
    public:
      void setup() override;
      void startRinging() override;
      void stopRinging() override;
      void setVolume(uint8_t volume) override;
      uint8_t getVolume() override;
      void setMute(bool mute) override;
      bool getMute() override;

    private:
      bool ringing = false;
      uint8_t volume = 100;
      bool mute = false;        
  };
}

#endif
