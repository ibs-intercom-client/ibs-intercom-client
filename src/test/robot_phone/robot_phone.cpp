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

#include <thread>
#include <chrono>
#include "robot_phone_options.h"
#include "robot_command_line_handler.h"
#include "phone_client.h"
#include <csignal>
#include <iostream>

void cleanup_on_signal(int signal);


static doorctrl::PhoneClient client;

void cleanup_on_signal(int signal) {
   client.hangupCall(); 
   std::cout << "cleanup triggered by signal " << signal << std::endl; 
   std::this_thread::sleep_for(std::chrono::seconds(2));
   exit(0);
}


int main(int argc, char* argv[]) {

   int cycle_count = 0; 
   doorctrl::RobotPhoneOptions options = doorctrl::RobotCommandLineHandler::handleArgs(argc, argv);

   client.setup(options);
   client.registerPhone();

   std::signal(SIGTERM, cleanup_on_signal); 
   std::signal(SIGINT, cleanup_on_signal); 

   while(true) {
      client.makeCall();

      std::this_thread::sleep_for(std::chrono::seconds(30));
      client.hangupCall();
      std::cout << "hangup..." << std::endl;
      cycle_count++;
      std::cout << "test cycle " << cycle_count << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(2));
   }
}
