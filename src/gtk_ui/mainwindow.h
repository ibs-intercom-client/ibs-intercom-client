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

#ifndef __DOORCTRL_MAINWINDOW_H__
#define __DOORCTRL_MAINWINDOW_H__

#include <gtk/gtk.h>
#include <memory>
#include "door_control_panel.h"
#include "video_panel.h"
#include "config/config_data.h"


namespace doorctrl {

  class VideoPanel;
  class DoorControlPanel;
  class UiControler;

  class GtkWidgetDeleter {
     public:
        void operator()(GtkWidget* widget) {
           gtk_widget_destroy(widget);
        }
  };

  class Mainwindow{
    public:
      explicit Mainwindow(GtkApplication* app_handle);
      void setup(std::shared_ptr<UiControler>& controler, std::shared_ptr<ConfigData>& config_data);
      void setupKioskMode();
      void show();
    
    private:
      GtkApplication* app_handle;

      /*
       * just delete top level GtkWidget, other widgets are released by 
       * auto iterating widget hierarchy. 
       */ 
      std::unique_ptr<GtkWidget, GtkWidgetDeleter> window_handle;  
      GtkBox* layout_box = nullptr; 
      std::shared_ptr<DoorControlPanel> doorctrl_panel;
      std::shared_ptr<VideoPanel> video_panel;
  };


}

#endif
