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

#ifndef __DOORCTRL_CONFIG_LOGGING_CONFIG_H__
#define __DOORCTRL_CONFIG_LOGGING_CONFIG_H__

#include <inttypes.h>
#include <string>
#include "common/severity_level.h"

namespace doorctrl {

  struct LoggingConfig {
    std::string logdir = "/var/log/doorctrl/";
    std::string logfile_prefix = "doorctrl";
    doorctrl::severity_level loglevel = doorctrl::error;
    doorctrl::severity_level console_loglevel = doorctrl::error;
    bool logging_disabled = false;
    uint32_t rotation_size = 10 * 1024 * 1024;

    bool equals(const LoggingConfig& other) const;
  }; 
}

#endif
