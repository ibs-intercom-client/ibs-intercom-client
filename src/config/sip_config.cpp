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

#include "sip_config.h"

bool doorctrl::SipConfig::equals(const SipConfig& other) const  {

  if(this->registrar.compare(other.registrar) != 0) return false;

  if(this->udp_port != other.udp_port) return false;

  if(this->audio_bind_address.compare(other.audio_bind_address) != 0) return false;

  if(this->audio_port_start != other.audio_port_start) return false;

  if(this->audio_port_range != other.audio_port_range) return false;

  if(this->reg_interval != other.reg_interval) return false;

  if(!this->credentials.equals(other.credentials)) return false;

  if(this->display_name.compare(other.display_name) != 0) return false;

  if(this->ec_enabled != other.ec_enabled) return false;

  if(this->ec_noise_suppressor_enabled != other.ec_noise_suppressor_enabled) return false;

  if(this->ec_tail_len != other.ec_tail_len) return false;

  if(this->ec_aggressiveness != other.ec_aggressiveness) return false;

  return true;
}
