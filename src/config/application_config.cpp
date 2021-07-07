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

#include "application_config.h"

bool doorctrl::ApplicationConfig::equals(const ApplicationConfig& other) const  {

  if(this->app_version != other.app_version) return false;

  if(this->app_name != other.app_name) return false;

  if(this->binary_name != other.binary_name) return false;

  if(this->kiosk_mode != other.kiosk_mode) return false;

  return true;
}
