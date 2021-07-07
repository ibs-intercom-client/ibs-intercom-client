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

#ifndef __DOORCTRL_HOMEGEAR_CONNECTOR_RPC_CLIENT_H__
#define __DOORCTRL_HOMEGEAR_CONNECTOR_RPC_CLIENT_H__

#include <homegear-base/BaseLib.h>
#include <shared_mutex>
#include <memory>
#include "config/config_data.h"

namespace doorctrl {

  class RpcClient {
    public:
      RpcClient();
      ~RpcClient();

      void setup(const std::shared_ptr<ConfigData>& config_data);
      void registerRpcMethod(const std::string& name, std::function<BaseLib::PVariable(const BaseLib::PArray &parameters)> func);
      bool isConnected();

      void start();
      void stop();
      void waitForStop();

      BaseLib::PVariable invoke(const std::string &method_name, const BaseLib::PArray &parameters, int32_t timeout);

    private:
      struct RequestInfo {
        std::mutex wait_mutex;
        std::condition_variable condition_variable;
      };

      struct RpcResponse {
        std::atomic_bool finished{false};
        BaseLib::PVariable response;
      };

      std::unique_ptr<BaseLib::SharedObjects> bl_;

      // Listener thread
      std::atomic_bool stop_listen_thread_{false};
      std::thread listen_thread_;

      // Socket
      std::shared_ptr<BaseLib::TcpSocket> socket_;
      std::atomic_bool reconnect_socket_{false};

      // Needed for "invoke()"
      std::mutex request_info_mutex_;
      RequestInfo request_info_;
      std::mutex rpc_responses_mutex_;
      RpcResponse rpc_response_;

      // Data processing
      std::unique_ptr<BaseLib::Rpc::BinaryRpc> binary_rpc_;
      std::unique_ptr<BaseLib::Rpc::RpcEncoder> rpc_encoder_;
      std::unique_ptr<BaseLib::Rpc::RpcDecoder> rpc_decoder_;

      // RPC methods
      std::map<std::string, std::function<BaseLib::PVariable(const BaseLib::PArray &parameters)>> local_rpc_methods_;

      void connect();
      void listen();

      void processRequest(const std::string &method_name, const BaseLib::PArray &parameters);
      void processResponse(const BaseLib::PVariable &response);

      // RPC methods
      BaseLib::PVariable ping(const BaseLib::PArray& parameters);
      BaseLib::PVariable listMethods(const BaseLib::PArray& parameters);

      std::shared_ptr<ConfigData> config_data;
  };
}

#endif 
