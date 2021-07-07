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

#ifndef __DOORCTRL_CONFIG_HOMEGEAR_RPC_CONFIG_H__
#define __DOORCTRL_CONFIG_HOMEGEAR_RPC_CONFIG_H__


#include <string>
#include <stdint.h>

namespace doorctrl {

  struct HomegearRpcConfig {
    bool enabled = false;
    std::string address = "";
    uint16_t port = 2001;
    std::string intercom_device_id = "Intercom";
    std::string doorbell_device_id = "Doorbell";
    std::string home_button_device_id = "Homebutton";

    bool equals(const HomegearRpcConfig& other) const;
  }; 
}

#endif
