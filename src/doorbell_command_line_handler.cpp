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

#include "doorbell_command_line_handler.h"
#include "doorbell_version.h"
#include "doorbell_appname.h"
#include "config/severity_level_helper.h"
#include <iostream>
#include <exception>
#include <cstring>


doorctrl::DoorbellCommandLineOptions doorctrl::DoorbellCommandLineHandler::handleArgs(int argc, char** argv) {

   doorctrl::DoorbellCommandLineOptions options;
   std::string operation = "";

   for(int i = 1; i < argc; i++) {

      if(strncmp(argv[i], "-h", 3) == 0 || strncmp(argv[i], "--help", 7) == 0) {
         if(operation.empty()) {
            operation = "help";
         }
      } else if(strncmp(argv[i], "-v", 3) == 0 || strncmp(argv[i], "--version", 10) == 0) {
         if(operation.empty()) {
            operation = "version";
         }
      } else if(strncmp(argv[i], "-d", 3) == 0 || strncmp(argv[i], "--console-debug", 16) == 0) {
         if(i < (argc - 1) && strlen(argv[i + 1]) == 1 && std::isdigit(*argv[i + 1])) {
            uint8_t console_debug_level = atoi(argv[i + 1]);
            doorctrl::SeverityLevelHelper helper(console_debug_level);

            options.disable_console_log = helper.isLoggingDisabled();
            options.console_debug = helper.getSeverityLevel();;

            i++;
         } else {
            if(i >= argc - 1) {
               std::cout << "the required argument for option '--console-debug' is missing" << std::endl;
            } else {
               std::cout << "the argument ('" << argv[i + 1] << "') for option '--console-debug' is invalid" << std::endl;
            }
            exit(1);
         }
      } else {

         std::cout << "unrecognized option '" << argv[i] << "'" << std::endl;
         exit(1);
      }
   }

   if(operation.compare("help") == 0) {
      printHelpText();
      exit(0);
   } else if(operation.compare("help") == 0) {
      printVersionText();
      exit(0);
   }


   return options;
}


void doorctrl::DoorbellCommandLineHandler::printVersionText() {
  std::cout << doorctrl::DOORBELL_APPNAME << " " << doorctrl::DOORBELL_VERSION << std::endl;
}


void doorctrl::DoorbellCommandLineHandler::printHelpText() {
  std::cout << doorctrl::DOORBELL_APPNAME << " " << doorctrl::DOORBELL_VERSION << std::endl;
  std::cout << "Usage: " << doorctrl::DOORBELL_BINNAME << " [OPTIONS]..." << std::endl;
  std::cout << "   -d, --console-debug=LEVEL  set console debug level:" << std::endl;
  std::cout << "                              0 = none, 1 = critical, ..., 5 = debug" << std::endl;
  std::cout << "                              fullscreen mode and hide window controls" << std::endl;
  std::cout << "   -v, --version              print doorbell version" << std::endl;
  std::cout << "   -h, --help                 print help message" << std::endl;
}

