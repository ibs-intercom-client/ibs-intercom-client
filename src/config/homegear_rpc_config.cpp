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

#include "homegear_rpc_config.h"

bool doorctrl::HomegearRpcConfig::equals(const HomegearRpcConfig& other) const  {

  if(this->address.compare(other.address) != 0) return false;

  if(this->enabled != other.enabled) return false;

  if(this->port != other.port) return false;

  if(this->intercom_device_id.compare(other.intercom_device_id) != 0) return false;
  if(this->doorbell_device_id.compare(other.doorbell_device_id) != 0) return false;
  if(this->home_button_device_id.compare(other.home_button_device_id) != 0) return false;

  return true;
}
