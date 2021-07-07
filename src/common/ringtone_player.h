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

#ifndef __DOORCTRL_COMMON_RINGTONE_PLAYER_H__
#define __DOORCTRL_COMMON_RINGTONE_PLAYER_H__

#include <inttypes.h>

namespace doorctrl {

  class RingtonePlayer {
     public:
        virtual ~RingtonePlayer() = default;
        virtual void setup() = 0;
        virtual void cleanup() = 0;
        virtual void startRinging() = 0;
        virtual void stopRinging() = 0;
        virtual void ringOnce() = 0;
        virtual void setVolume(uint8_t percent) = 0;
        virtual uint8_t getVolume() = 0;
        virtual void setMute(bool mute) = 0;
        virtual bool getMute() = 0;
  };

} 

#endif
