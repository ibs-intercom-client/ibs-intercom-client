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

#ifndef __DOORCTRL_GTK_UI_H__
#define __DOORCTRL_GTK_UI_H__

#include <gtk/gtk.h>
#include <memory>
#include "common/phone.h"
#include "common/door_opener.h"
#include "config/config_data.h"
#include "common/home_button.h"



namespace doorctrl {

  class GtkUi {
    public:
      GtkUi() = delete;
      GtkUi(const GtkUi& ui) = delete;
      GtkUi& operator=(const GtkUi& ui) = delete;
   
      static void setup(std::shared_ptr<Phone>& phone,
                        std::shared_ptr<DoorOpener>& door_opener,
                        std::shared_ptr<HomeButton>& home_button,
                        std::shared_ptr<ConfigData>& config_data);

      static int enterMainLoop(int argc, char** argv);

      static void handleActivate(GtkApplication* app, gpointer user_data);
    
  };
}



#endif



