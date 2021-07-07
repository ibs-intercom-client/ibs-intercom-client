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

#include "door_control_button_monkey.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

static boost::random::mt19937 gen;
static boost::random::uniform_int_distribution<> yes_no(1, 2);
static boost::random::uniform_int_distribution<> one_of_five(1, 5);

static gboolean run_test_increment(gpointer user_data) {
   doorctrl::DoorControlButtonMonkey* monkey = static_cast<doorctrl::DoorControlButtonMonkey*>(user_data);

   if(monkey != nullptr) {
      monkey->runTestIncrement();
   }

   return true;
}


doorctrl::DoorControlButtonMonkey::~DoorControlButtonMonkey() {
  this->stop();
}


void doorctrl::DoorControlButtonMonkey::setActivityButton(GtkWidget* activity_button) {
  this->activity_button = activity_button;
}


void doorctrl::DoorControlButtonMonkey::setHangupButton(GtkWidget* hangup_button) {
  this->hangup_button = hangup_button;
}


void doorctrl::DoorControlButtonMonkey::start() {
   gdk_threads_add_timeout(1000, &run_test_increment, this);
   this->stopping = false;
}


void doorctrl::DoorControlButtonMonkey::stop() {

   this->stopping = true;
}


bool doorctrl::DoorControlButtonMonkey::isStopping() const {
   return this->stopping;
}


void doorctrl::DoorControlButtonMonkey::runTestIncrement() {

  std::vector<GtkWidget*> visible_buttons;

  if(this->activity_button != nullptr && gtk_widget_is_visible(this->activity_button)) {
     visible_buttons.push_back(this->activity_button);
  }

  if(this->hangup_button != nullptr && gtk_widget_is_visible(this->hangup_button)) {
     visible_buttons.push_back(this->hangup_button);
  }

  /*
   * to do or not to do ?
   */
  if(one_of_five(gen) == 1) {

     if(visible_buttons.size() > 1) {
        boost::random::uniform_int_distribution<> dist(1, visible_buttons.size());
  
        int index = dist(gen) - 1;

        gtk_button_clicked(GTK_BUTTON(visible_buttons[index]));
//        gtk_button_clicked(GTK_BUTTON(this->activity_button));
     } else if(visible_buttons.size() == 1) {
        gtk_button_clicked(GTK_BUTTON(visible_buttons[0]));
//        gtk_button_clicked(GTK_BUTTON(this->activity_button));
     }
  }
}
