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

#ifndef __DOORCTRL_GTK_UI_X_WINDOW_CONTROLER_H__
#define __DOORCTRL_GTK_UI_X_WINDOW_CONTROLER_H__

#include <string>

namespace doorctrl {

   class XWindowControler {
     public:
   
       static void setup();
       static void wakeupScreen();
       static void bringToFront(const std::string& window_title);
       static std::string getActiveWindowTitle();
       static void disableScreensaver();
       static void enableScreensaver();
   };
}

#endif
