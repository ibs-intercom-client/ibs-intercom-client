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

#include "severity_level_helper.h"


doorctrl::SeverityLevelHelper::SeverityLevelHelper(uint8_t config_debug_level) {

  if(config_debug_level == 0) {
     this->logging_disabled = true;;
  }

  if(config_debug_level > 5) {
    this->severity_level = doorctrl::fine;
  } else {
    switch(config_debug_level) {
      case 1:
        this->severity_level = doorctrl::critical;
        break;
      case 2:
        this->severity_level = doorctrl::error;
        break;
      case 3:
        this->severity_level = doorctrl::warning;
        break;
      case 4:
        this->severity_level = doorctrl::info;
        break;
      case 5:
        this->severity_level = doorctrl::debug;
        break;
      default:
        this->severity_level = doorctrl::error;
    }
  }
}


doorctrl::severity_level doorctrl::SeverityLevelHelper::getSeverityLevel() const {
   return this->severity_level;
}


bool doorctrl::SeverityLevelHelper::isLoggingDisabled() const {
   return this->logging_disabled;
}

