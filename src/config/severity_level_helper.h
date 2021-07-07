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

#ifndef __DOORCTRL_CONFIG_SEVERITY_LEVEL_HELPER_H__
#define __DOORCTRL_CONFIG_SEVERITY_LEVEL_HELPER_H__

#include <inttypes.h>
#include "common/severity_level.h"

namespace doorctrl {

  class SeverityLevelHelper {
    public:
      explicit SeverityLevelHelper(uint8_t config_debug_level);
      doorctrl::severity_level getSeverityLevel() const;
      bool isLoggingDisabled() const;

    private:
      doorctrl::severity_level severity_level = doorctrl::error;
      bool logging_disabled = false;
  };
}

#endif
