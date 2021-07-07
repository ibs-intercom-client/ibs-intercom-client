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

#ifndef __DOORCTRL_COMMON_HOME_BUTTON_H__
#define __DOORCTRL_COMMON_HOME_BUTTON_H__

#include "common/home_button_state_observer.h"

namespace doorctrl {

    class HomeButton {
      public:
        virtual ~HomeButton() = default;
        virtual bool isPressed() const = 0;
        virtual void addObserver(const std::shared_ptr<HomeButtonStateObserver>& observer) = 0;
        virtual void removeObserver(const std::shared_ptr<HomeButtonStateObserver>& observer) = 0;
    };
}

#endif
