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

#include "rpc_client.h"

#include <homegear-base/BaseLib.h>
#include <memory>
#include <iostream>
#include <chrono>
#include <thread>
#include "rpc_event_connector.h"


int main(int argc, char* argv[]) {

   std::shared_ptr<doorctrl::RpcClient> client = std::make_shared<doorctrl::RpcClient>(); 
   doorctrl::RpcEventConnector connector;
   connector.setup(client);

   std::shared_ptr<doorctrl::ConfigData> config_data = std::make_shared<doorctrl::ConfigData>();
   doorctrl::HomegearRpcConfig rpc_config;
   rpc_config.address = "192.168.200.153"; 
   rpc_config.port = 2001; 
   config_data->updateHomegearRpcConfig(rpc_config);
   client->setup(config_data);

   client->start();

   while(!client->isConnected()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
   }

   BaseLib::PArray params;
   std::shared_ptr<BaseLib::Variable> version = client->invoke("getVersion", params, 1000);

   std::cout << version->toString() << std::endl;
   version->print(true);

   BaseLib::PArray init_params = std::make_shared<BaseLib::Array>();
//   init_params->push_back(std::make_shared<BaseLib::Variable>("binary://192.168.200.160:1234"));
   init_params->push_back(std::make_shared<BaseLib::Variable>(""));
   init_params->push_back(std::make_shared<BaseLib::Variable>("doorctrl"));
   init_params->push_back(std::make_shared<BaseLib::Variable>(7));
   std::shared_ptr<BaseLib::Variable> result = client->invoke("init", init_params, 1000);
   result->print(true);
    
   std::this_thread::sleep_for(std::chrono::seconds(15));

   client->stop();
   client->waitForStop();
}


