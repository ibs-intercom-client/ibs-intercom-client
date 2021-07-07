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

#ifndef __DOORCTRL_CONFIG_PHONE_CONFIG_H__
#define __DOORCTRL_CONFIG_PHONE_CONFIG_H__

#include <inttypes.h>

namespace doorctrl {

  struct PhoneConfig {
    uint8_t volume = 100;
    uint8_t mic_level = 100;
    bool mute_mic = false;

    bool equals(const PhoneConfig& other) const;
  }; 
}

#endif
