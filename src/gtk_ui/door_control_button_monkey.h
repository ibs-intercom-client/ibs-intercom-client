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

#ifndef __DOORCTRL_GTK_UI_DOOR_CTRL_BUTTON_MONKEY_H__
#define __DOORCTRL_GTK_UI_DOOR_CTRL_BUTTON_MONKEY_H__

#include <mutex>
#include <atomic>
#include <gtk/gtk.h>

namespace doorctrl {

   class DoorControlButtonMonkey {
     public:
       ~DoorControlButtonMonkey();
       void setActivityButton(GtkWidget* action_button);
       void setHangupButton(GtkWidget* hangup_button);
       void start();
       void stop();
       bool isStopping() const;
       void runTestIncrement();

     private:
       GtkWidget* activity_button = nullptr;
       GtkWidget* hangup_button = nullptr;
       std::mutex monkey_mutex;
       std::atomic<bool> stopping; 
   };
}

#endif
