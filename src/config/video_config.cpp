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

#include "video_config.h"

bool doorctrl::VideoConfig::equals(const VideoConfig& other) const  {

  if(this->video_url.compare(other.video_url) != 0) return false;

  if(this->enabled != other.enabled) return false;

  if(this->fade_out_seconds != other.fade_out_seconds) return false;

  return true;
}
