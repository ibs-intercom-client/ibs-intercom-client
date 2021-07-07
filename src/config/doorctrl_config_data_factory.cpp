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

#include "doorctrl_config_data_factory.h"
#include "config_data_file_reader.h"

static const std::string GLOBAL_CONFIG_FILE = "/etc/doorctrl/doorctrl.conf";
static const std::string USER_CONFIG_FILE = ".doorctrl.conf";

doorctrl::DoorctrlConfigDataFactory::DoorctrlConfigDataFactory() 
{

}


doorctrl::DoorctrlConfigDataFactory& doorctrl::DoorctrlConfigDataFactory::getInstance() {
  static DoorctrlConfigDataFactory instance;
  return instance;
} 


std::shared_ptr<doorctrl::ConfigData> doorctrl::DoorctrlConfigDataFactory::getConfigData() {
   auto config_data = std::make_shared<doorctrl::ConfigData>(); 
   doorctrl::ConfigDataFileReader::readConfigData(config_data, GLOBAL_CONFIG_FILE, USER_CONFIG_FILE);
   return config_data; 
}

