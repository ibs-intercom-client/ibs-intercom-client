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

#include "logging_config.h"

bool doorctrl::LoggingConfig::equals(const LoggingConfig& other) const  {

  if(this->logdir.compare(other.logdir) != 0) return false;

  if(this->logfile_prefix.compare(other.logfile_prefix) != 0) return false;

  if(this->loglevel != other.loglevel) return false;

  if(this->console_loglevel != other.console_loglevel) return false;

  if(this->logging_disabled != other.logging_disabled) return false;

  if(this->rotation_size != other.rotation_size) return false;

  return true;
}
