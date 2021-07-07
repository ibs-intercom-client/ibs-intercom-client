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

#ifndef __DOORCTRL_HOMEGEAR_CONNECT_RPC_EVENT_CONNECTOR_H__
#define __DOORCTRL_HOMEGEAR_CONNECT_RPC_EVENT_CONNECTOR_H__

#include <memory>
#include <homegear-base/BaseLib.h>
#include "rpc_client.h"

namespace doorctrl {

  class RpcEventConnector {
    public:
      void setup(const std::shared_ptr<RpcClient>& rpc_client);

    private:
      BaseLib::PVariable event(const BaseLib::PArray &parameters);
      std::shared_ptr<RpcClient> rpc_client;
  };
}

#endif
