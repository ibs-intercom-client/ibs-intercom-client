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

#ifndef __DOORCTRL_DOORCTRL_COMMAND_LINE_OPTIONS_H__
#define __DOORCTRL_DOORCTRL_COMMAND_LINE_OPTIONS_H__

#include "common/severity_level.h"

namespace doorctrl {

  struct DoorctrlCommandLineOptions {
    bool kiosk_mode = 0;
    bool disable_console_log = false;
    doorctrl::severity_level console_debug = doorctrl::error;   
  };
}

#endif
