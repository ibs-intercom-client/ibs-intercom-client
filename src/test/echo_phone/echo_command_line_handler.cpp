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

#include "echo_command_line_handler.h"
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <iostream>
#include <exception>

doorctrl::EchoPhoneOptions doorctrl::EchoCommandLineHandler::handleArgs(int argc, char** argv) {

   doorctrl::EchoPhoneOptions options;

   try {

      boost::program_options::options_description desc("Options");
      desc.add_options()
         ("help", "print help message")
         ("registrar", boost::program_options::value<std::string>(&options.registrar), "address of SIP registrar server")
         ("port", boost::program_options::value<uint16_t>(&options.port), "udp port, default = 5060")
         ("user", boost::program_options::value<std::string>(&options.user), "user name at registrar server")
         ("passwd", boost::program_options::value<std::string>(&options.passwd), "user password")
         ("input", boost::program_options::value<std::string>(&options.input), "input file")
         ("output", boost::program_options::value<std::string>(&options.output), "output file")
      ;

      boost::program_options::variables_map vm;
      boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
      boost::program_options::notify(vm);    

      if (vm.count("help") > 0) {
         std::cout << "robot_phone 0.0.1" << std::endl;
         std::cout << "Usage: robot_phone [OPTIONS]..." << std::endl;
         std::cout << desc << std::endl;
         exit(1);
      } else if (vm.count("version") > 0) {
         std::cout << "robot_phone 0.0.1" << std::endl;
         exit(1);
      } else if (options.registrar.empty()) {
         std::cout << "registrar address is missing, please add option --registrar ADDRESS" << std::endl;
         exit(1);
      } else if (options.input.empty()) {
         std::cout << "input file name is missing, please add option --input INPUT FILE" << std::endl;
         exit(1);
      } else if (options.output.empty()) {
         std::cout << "output file name is missing, please add option --output OUTPUT FILE" << std::endl;
         exit(1);
      } else if (options.user.empty()) {
         std::cout << "user name is missing, please add option --user USER" << std::endl;
         exit(1);
      }
   } catch (std::exception& ex) {
      std::cout << ex.what() << std::endl;
      exit(1);
   }

   return options;
}

