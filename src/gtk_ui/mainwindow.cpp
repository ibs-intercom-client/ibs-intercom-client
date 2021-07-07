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

#include "mainwindow.h"
#include "ui_controler.h"
#include <memory>

static gboolean on_mainwindow_destroy(GtkWidget* widget, GdkEvent* event, gpointer data);


gboolean on_mainwindow_destroy(GtkWidget* widget, GdkEvent* event, gpointer data) {
   doorctrl::UiControler* controler = static_cast<doorctrl::UiControler*>(data);

   controler->doHangup();
   return FALSE;
}


doorctrl::Mainwindow::Mainwindow(GtkApplication* app_handle) 
  : app_handle(app_handle) {


}


void doorctrl::Mainwindow::setup(std::shared_ptr<UiControler>& controler, std::shared_ptr<ConfigData>& config_data) {

   this->window_handle.reset(gtk_application_window_new(this->app_handle));

   gtk_widget_set_name(GTK_WIDGET (this->window_handle.get()), (gchar*)"mainwindow"); 
   gtk_window_set_title(GTK_WINDOW (this->window_handle.get()), (gchar*)config_data->getApplicationConfig().app_name.c_str()); 

   /*
    * The aspect ratio of the touch panels display is 16:10 
    */
   gtk_window_set_default_size(GTK_WINDOW (this->window_handle.get()), 800, 600); 

   this->layout_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
   gtk_widget_set_name(GTK_WIDGET (this->layout_box), (gchar*)"main_layout_box"); 
   gtk_container_add (GTK_CONTAINER(this->window_handle.get()), GTK_WIDGET(this->layout_box));

     this->video_panel = std::make_shared<VideoPanel>(config_data);
     this->video_panel->addObserver(controler);
     this->video_panel->setup();

     gtk_box_pack_start(GTK_BOX(this->layout_box), this->video_panel->getHandle(), TRUE, TRUE, 0); 
     gtk_widget_realize(this->video_panel->getHandle());


   this->doorctrl_panel = std::make_shared<DoorControlPanel>();
   this->doorctrl_panel->setup(controler);

   gtk_box_pack_start(GTK_BOX(this->layout_box), this->doorctrl_panel->getHandle(), FALSE, FALSE, 10); 

   if(controler != nullptr) {
      controler->setDoorControlPanel(this->doorctrl_panel);
      controler->setVideoPanel(this->video_panel);
   }

   g_signal_connect(G_OBJECT(this->window_handle.get()), "delete-event", G_CALLBACK(on_mainwindow_destroy), controler.get());
}


void doorctrl::Mainwindow::setupKioskMode() {
   if(this->window_handle !=  nullptr) {
      gtk_window_fullscreen(GTK_WINDOW(this->window_handle.get()));
      gtk_window_set_decorated(GTK_WINDOW(this->window_handle.get()), FALSE);
   } 
}


void doorctrl::Mainwindow::show() {
   gtk_widget_show_all(this->window_handle.get()); 
}
