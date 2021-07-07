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

#include "rpc_event_connector.h"


void doorctrl::RpcEventConnector::setup(const std::shared_ptr<doorctrl::RpcClient>& rpc_client) {
  this->rpc_client = rpc_client;

  this->rpc_client->registerRpcMethod("event", std::bind(&doorctrl::RpcEventConnector::event, this, std::placeholders::_1));

  BaseLib::PArray init_params = std::make_shared<BaseLib::Array>();
//  init_params->push_back(std::make_shared<BaseLib::Variable>("binary://192.168.200.101:1234"));
  init_params->push_back(std::make_shared<BaseLib::Variable>(""));
  init_params->push_back(std::make_shared<BaseLib::Variable>("doorctrl"));
  init_params->push_back(std::make_shared<BaseLib::Variable>(7));
  std::shared_ptr<BaseLib::Variable> result = this->rpc_client->invoke("init", init_params, 1000);
}


BaseLib::PVariable doorctrl::RpcEventConnector::event(const BaseLib::PArray &parameters) {

  try {

    return std::make_shared<BaseLib::Variable>("Hallo Welt");

  } catch(const std::exception& ex) {

  }

  return BaseLib::Variable::createError(-32500, "Unknown application error.");
}
