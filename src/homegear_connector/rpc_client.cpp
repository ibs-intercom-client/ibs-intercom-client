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
#include "homegear_connector_logger.h"

doorctrl::RpcClient::RpcClient() {
  try {
    signal(SIGPIPE, SIG_IGN);

    bl_ = std::make_unique<BaseLib::SharedObjects>();

    binary_rpc_ = std::make_unique<BaseLib::Rpc::BinaryRpc>(bl_.get());
    rpc_encoder_ = std::make_unique<BaseLib::Rpc::RpcEncoder>(bl_.get(), true, true);
    rpc_decoder_ = std::make_unique<BaseLib::Rpc::RpcDecoder>(bl_.get(), false, false);

  } catch (const std::exception &ex) {
    doorctrl::HomegearConnectorLogger::getInstance().getOutput().printError("Error: error while creating rpc client instance");
  }
}


doorctrl::RpcClient::~RpcClient() {
  stop();
}


void doorctrl::RpcClient::setup(const std::shared_ptr<ConfigData>& config_data) {

   this->config_data = config_data; 

   this->local_rpc_methods_.emplace("ping", std::bind(&doorctrl::RpcClient::ping, this, std::placeholders::_1));
   this->local_rpc_methods_.emplace("system.listMethods", std::bind(&doorctrl::RpcClient::listMethods, this, std::placeholders::_1));
}


void doorctrl::RpcClient::registerRpcMethod(const std::string& name, std::function<BaseLib::PVariable(const BaseLib::PArray &parameters)> func) {
   this->local_rpc_methods_.emplace(name, func);
} 


bool doorctrl::RpcClient::isConnected() {
  return socket_ && socket_->connected();
}


void doorctrl::RpcClient::start() {
  stop_listen_thread_.store(false, std::memory_order_release);
  reconnect_socket_.store(true, std::memory_order_release);
  listen_thread_ = std::thread(&doorctrl::RpcClient::listen, this);
}


void doorctrl::RpcClient::stop() {
  try {
    stop_listen_thread_.store(true, std::memory_order_release);
  } catch (const std::exception &ex) {
    doorctrl::HomegearConnectorLogger::getInstance().getOutput().printError("Error: error while stopping rpc client, message = " + std::string(ex.what()));
  }
}


void doorctrl::RpcClient::waitForStop() {
  try {
    if (listen_thread_.joinable()) listen_thread_.join();

    if (socket_) {
      socket_->close();
      socket_.reset();
    }
  } catch (const std::exception &ex) {
    doorctrl::HomegearConnectorLogger::getInstance().getOutput().printError("Error: error while waiting for rpc client stop, message = " + std::string(ex.what()));
  }
}


void doorctrl::RpcClient::connect() {
  try {
    if (socket_) socket_->close();

    if(this->config_data == nullptr) {
       doorctrl::HomegearConnectorLogger::getInstance().getOutput().printCritical("Critical: config data not set in rpc client, exiting...");
       exit(1);
    }

    std::string hostname = this->config_data->getHomegearRpcConfig().address; 
    std::string port = "2001";
    
    socket_ = std::make_shared<BaseLib::TcpSocket>(bl_.get(), hostname, port);
//    socket_ = std::make_shared<BaseLib::TcpSocket>(bl_.get(), hostname, port, true, caFile, true, clientCertificate, clientKey);
    socket_->open();
    doorctrl::HomegearConnectorLogger::getInstance().getOutput().printInfo("Info: Connected to RPC server " + hostname + " on port " + port);
    socket_->setReadTimeout(1000000);
    socket_->setWriteTimeout(120000000);

    reconnect_socket_.store(false, std::memory_order_release);
  }
  catch (const std::exception &ex) {
    doorctrl::HomegearConnectorLogger::getInstance().getOutput().printError("Error: error connecting homegear rpc server");
    if (socket_) socket_->close();
  }
}

void doorctrl::RpcClient::listen() {
  try {
    try {
      connect();
    }
    catch (const std::exception &ex) {
      doorctrl::HomegearConnectorLogger::getInstance().getOutput().printWarning("Warning: Could not connect to RPC server, message = " +  std::string(ex.what()));
      reconnect_socket_.store(true, std::memory_order_release);
    }

    std::array<uint8_t, 4096> buffer{};
    int32_t read_bytes = 0;

    while (!stop_listen_thread_.load(std::memory_order_acquire)) {
      try {
        if (reconnect_socket_.load(std::memory_order_acquire)) {
          if (stop_listen_thread_.load(std::memory_order_acquire)) return;
          std::this_thread::sleep_for(std::chrono::milliseconds(1000));

          doorctrl::HomegearConnectorLogger::getInstance().getOutput().printDebug("Debug: Reconnecting RPC server");

          connect();
        }

        try {
          if (!socket_ || !socket_->connected()) continue;
          read_bytes = socket_->proofread((char *)buffer.data(), buffer.size());
        }
        catch (BaseLib::SocketClosedException &ex) {
          doorctrl::HomegearConnectorLogger::getInstance().getOutput().printInfo("Info: Connection to RPC server closed");
          reconnect_socket_.store(true, std::memory_order_release);
          continue;
        }
        catch (BaseLib::SocketTimeOutException &ex) {
          continue;
        }

        if (read_bytes <= 0) continue; //Shouldn't happen

        doorctrl::HomegearConnectorLogger::getInstance().getOutput().printDebug("Debug: Raw packet received from RPC server: " + BaseLib::HelperFunctions::getHexString((char*)buffer.data(), read_bytes));

        int32_t processed_bytes = 0;

        while (processed_bytes < read_bytes) {
          processed_bytes += binary_rpc_->process((char *)buffer.data() + processed_bytes, buffer.size() - processed_bytes);
          if (binary_rpc_->isFinished()) {
            if (binary_rpc_->getType() == BaseLib::Rpc::BinaryRpc::Type::request) {
              std::string method_name;
              auto parameters = rpc_decoder_->decodeRequest(binary_rpc_->getData(), method_name);
              processRequest(method_name, parameters);
            } else if (binary_rpc_->getType() == BaseLib::Rpc::BinaryRpc::Type::response) {
              auto response = rpc_decoder_->decodeResponse(binary_rpc_->getData());
              processResponse(response);
            }

            binary_rpc_->reset();
          }
        }
      }
      catch (const std::exception &ex) {
        doorctrl::HomegearConnectorLogger::getInstance().getOutput().printInfo("Info: error while waiting for data from RPC server, message = " + std::string(ex.what()));
        reconnect_socket_.store(true, std::memory_order_release);
      }
    }
  }
  catch (const std::exception &ex) {
     doorctrl::HomegearConnectorLogger::getInstance().getOutput().printInfo("Info: error while waiting for data from RPC server, message = " +  std::string(ex.what()));
  }
}


void doorctrl::RpcClient::processRequest(const std::string &method_name, const BaseLib::PArray &parameters) {
  try {
    doorctrl::HomegearConnectorLogger::getInstance().getOutput().printInfo("Info: Remote partner (client) is calling RPC method " +  method_name);
    doorctrl::HomegearConnectorLogger::getInstance().getOutput().printDebug("Debug: Parameters:\n" + std::make_shared<BaseLib::Variable>(parameters)->print(false, false));

    BaseLib::PVariable result;

    auto method_iterator = local_rpc_methods_.find(method_name);
    if (method_iterator == local_rpc_methods_.end()) {
      doorctrl::HomegearConnectorLogger::getInstance().getOutput().printWarning("Warning: requested RPC method " + method_name + " not found, sending error response");
      result = BaseLib::Variable::createError(-32601, "Requested method not found.");
    } else {
      result = method_iterator->second(parameters);
    }

    std::vector<char> response_packet;
    rpc_encoder_->encodeResponse(result, response_packet);
    socket_->proofwrite(response_packet);
  }
  catch (const std::exception &ex) {
    doorctrl::HomegearConnectorLogger::getInstance().getOutput().printError("Error: errror while processing RPC method " + method_name + ", message = " + std::string(ex.what()));
  }
}

void doorctrl::RpcClient::processResponse(const BaseLib::PVariable &response) {
  try {
    std::lock_guard<std::mutex> request_info_guard(request_info_mutex_);
    std::unique_lock<std::mutex> wait_lock(request_info_.wait_mutex);

    {
      std::lock_guard<std::mutex> response_guard(rpc_responses_mutex_);
      rpc_response_.response = response;
      rpc_response_.finished = true;
    }

    wait_lock.unlock();
    request_info_.condition_variable.notify_all();
  }
  catch (const std::exception &ex) {
    doorctrl::HomegearConnectorLogger::getInstance().getOutput().printError("Error: errror while processing RPC response, message = " + std::string(ex.what()));
  }
}


BaseLib::PVariable doorctrl::RpcClient::invoke(const std::string &method_name, const BaseLib::PArray &parameters, int32_t timeout) {
  try {
    if (!socket_ || !socket_->connected()) {
      doorctrl::HomegearConnectorLogger::getInstance().getOutput().printWarning("Warning: error while invoking rpc method " + method_name + ", there is no open RPC connection");
      return BaseLib::Variable::createError(-32500, "Unknown application error.");
    }

    std::vector<char> data;
    rpc_encoder_->encodeRequest(method_name, parameters, data);

    rpc_response_.finished = false;

    try {
      socket_->proofwrite(data);
    }
    catch (const std::exception &ex) {
      doorctrl::HomegearConnectorLogger::getInstance().getOutput().printError("Error: error while invoking rpc method " + method_name + ", message = " + std::string(ex.what()));

      return BaseLib::Variable::createError(-32500, "Unknown application error.");
    }

    auto start_time = BaseLib::HelperFunctions::getTime();
    std::unique_lock<std::mutex> wait_lock(request_info_.wait_mutex);
    while (!request_info_.condition_variable.wait_for(wait_lock, std::chrono::milliseconds(1000), [&] {
      return rpc_response_.finished || stop_listen_thread_ || (timeout > 0 && BaseLib::HelperFunctions::getTime() - start_time > timeout);
    }));

    BaseLib::PVariable result;
    if (!rpc_response_.finished || !rpc_response_.response) {
      //Gd::out.printError("Error: No response received to RPC request. Method: " + methodName);
      result = BaseLib::Variable::createError(-1, "No response received.");
    } else result = rpc_response_.response;

    {
      std::lock_guard<std::mutex> response_guard(rpc_responses_mutex_);
      rpc_response_.response.reset();
    }

    return result;
  }
  catch (const std::exception &ex) {
    doorctrl::HomegearConnectorLogger::getInstance().getOutput().printError("Error: error while invoking rpc method " + method_name + ", message = " + std::string(ex.what()));
  }
  return BaseLib::Variable::createError(-32500, "Unknown application error.");
}


BaseLib::PVariable doorctrl::RpcClient::ping(const BaseLib::PArray& parameters)
{
  try
  {
    return std::make_shared<BaseLib::Variable>(BaseLib::HelperFunctions::getTime());
  }
  catch(const std::exception& ex)
  {
    //Todo: Handle exception
    //Gd::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
  }
  return BaseLib::Variable::createError(-32500, "Unknown application error.");
}


BaseLib::PVariable doorctrl::RpcClient::listMethods(const BaseLib::PArray& parameters) {
  doorctrl::HomegearConnectorLogger::getInstance().getOutput().printDebug("Debug: list implemented RPC methods");
  try {

    std::shared_ptr<BaseLib::Array> method_names = std::make_shared<BaseLib::Array>(); 

    for(auto item : this->local_rpc_methods_) {
       doorctrl::HomegearConnectorLogger::getInstance().getOutput().printDebug("Debug: adding local RPC method " +  item.first);
       method_names->push_back(std::make_shared<BaseLib::Variable>(item.first));
    }

    return std::make_shared<BaseLib::Variable>(method_names);

  } catch(const std::exception& ex) {
    //Todo: Handle exception
    //Gd::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
  }

  return BaseLib::Variable::createError(-32500, "Unknown application error.");
}
