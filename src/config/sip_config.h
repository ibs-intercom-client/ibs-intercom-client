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

#ifndef __DOORCTRL_CONFIG_SIP_CONFIG_H__
#define __DOORCTRL_CONFIG_SIP_CONFIG_H__


#include <string>
#include <stdint.h>
#include "common/credentials.h"

namespace doorctrl {

  /*
   * SIP configuration data.
   * The registrar is the server running the SIP server e.g. Asterisk.
   * Echo cancellation (EC) settings are rather special to PJSIP which is configured
   * to use the SPEEX algorithm. Aggressiveness has four settings: 0 = off, 1, 2, 3 = strong
   * The default values are either usual values or invalid values to avoid
   * accidental misconfigurations of the system.
   */
  struct SipConfig {
    std::string registrar = "";
    uint16_t udp_port = 5060;
    uint16_t audio_port_start = 10000;
    uint16_t audio_port_range = 10000;
    std::string audio_bind_address = "0.0.0.0";
    uint16_t reg_interval = 300;
    Credentials credentials;
    std::string display_name = "";
    uint16_t ec_tail_len = 500;
    bool ec_enabled = true;
    bool ec_noise_suppressor_enabled = true;
    uint8_t ec_aggressiveness = 0;

    bool equals(const SipConfig& other) const;
  }; 
}

#endif
