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

#include "ringtone_config.h"

bool doorctrl::RingtoneConfig::equals(const RingtoneConfig& other) const  {

  if(this->path.compare(other.path) != 0) return false;

  if(this->volume != other.volume) return false;

  if(this->mute != other.mute) return false;

  if(this->duration != other.duration) return false;

  return true;
}
