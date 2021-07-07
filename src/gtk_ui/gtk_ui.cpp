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

#include "gtk_ui.h"
#include "mainwindow.h"
#include "ui_controler.h"
#include <memory>
#include <X11/Xlib.h>

static GtkApplication* app_handle;
static std::unique_ptr<doorctrl::Mainwindow> g_mainwindow;
static std::shared_ptr<doorctrl::UiControler> g_controler;
static std::shared_ptr<doorctrl::ConfigData> g_config_data; 


void doorctrl::GtkUi::setup(std::shared_ptr<Phone>& phone,
                            std::shared_ptr<DoorOpener>& door_opener,
                            std::shared_ptr<HomeButton>& home_button,
                            std::shared_ptr<ConfigData>& config_data) {

//   XInitThreads();

   gtk_init(config_data->getApplicationConfig().argc_ptr, config_data->getApplicationConfig().argv_ptr);

   app_handle = gtk_application_new("eu.homegear.doorctrl", G_APPLICATION_FLAGS_NONE);

   g_signal_connect(app_handle, "activate", G_CALLBACK(doorctrl::GtkUi::handleActivate), NULL); 

   g_mainwindow = std::make_unique<doorctrl::Mainwindow>(app_handle);

   g_controler = std::make_shared<UiControler>(phone, door_opener, home_button);
   g_controler->setup(config_data);

   GtkCssProvider *cssProvider = gtk_css_provider_new();
   gtk_css_provider_load_from_path(cssProvider, "/usr/share/doorctrl/styles/doorctrl_dark_theme.css", NULL);
   gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                             GTK_STYLE_PROVIDER(cssProvider),
                                             GTK_STYLE_PROVIDER_PRIORITY_USER);


   g_config_data = config_data;
}


int doorctrl::GtkUi::enterMainLoop(int argc, char** argv) {

  int status = g_application_run(G_APPLICATION(app_handle), argc, argv);

  return status;
}


void doorctrl::GtkUi::handleActivate(GtkApplication* app, gpointer user_data) {

  g_mainwindow->setup(g_controler, g_config_data);

  if(g_config_data->getApplicationConfig().kiosk_mode) {
    g_mainwindow->setupKioskMode();
  }

  g_config_data->addObserver(g_controler);
  g_controler->updateHighlightColor();

  g_mainwindow->show();
  g_controler->enterInitialState(); 
}
