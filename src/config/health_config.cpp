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

#include "health_config.h"

bool doorctrl::HealthConfig::equals(const HealthConfig& other) const  {

  if(this->http_bind_address.compare(other.http_bind_address) != 0) return false;

  if(this->enabled != other.enabled) return false;

  if(this->http_port != other.http_port) return false;

  if(this->health_collector_sleep != other.health_collector_sleep) return false;

  return true;
}
