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

#ifndef __DOORCTRL_CONFIG_CONFIG_DATA_FILE_READER_H__
#define __DOORCTRL_CONFIG_CONFIG_DATA_FILE_READER_H__

#include <memory>
#include "config_data.h"

namespace doorctrl {

  class ConfigDataFileReader {
    public:
      static void readConfigData(std::shared_ptr<ConfigData>& config_data, const std::string& global_file_path, const std::string& user_file_name);     
  };
}

#endif
