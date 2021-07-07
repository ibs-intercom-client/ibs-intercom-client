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

#ifndef __DOORCTRL_CONFIG_CONGIG_DATA_CHANGED_OBSERVER_H__
#define __DOORCTRL_CONFIG_CONGIG_DATA_CHANGED_OBSERVER_H__

#include "config_data_changed_event.h"

namespace doorctrl {

  class ConfigDataChangedObserver {
    public:
      virtual ~ConfigDataChangedObserver() = default;
      virtual void handleEvent(const ConfigDataChangedEvent& event) = 0;
  };
}

#endif
