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

#include "http_server.h"
#include "http_interface_logger.h"
#include "common/baselib_manager.h"
#include <mutex>

std::mutex request_mutex;
static std::shared_ptr<doorctrl::HttpRequestHandler> null_handler;

void doorctrl::HttpServer::addRequestHandler(const std::shared_ptr<doorctrl::HttpRequestHandler>& handler) {
   doorctrl::HttpInterfaceLogger::getInstance().getOutput().printDebug("Debug: registering new http request handler, url = " + handler->getUrl());

   std::lock_guard<std::mutex> lock(request_mutex);
   this->request_handler_list.push_back(handler);
}

std::shared_ptr<doorctrl::HttpRequestHandler> doorctrl::HttpServer::getRequestHandler(const std::string& url) const {
   doorctrl::HttpInterfaceLogger::getInstance().getOutput().printDebug("Debug: get http request handler, request url = " + url);
   std::lock_guard<std::mutex> lock(request_mutex);

   for(auto current : this->request_handler_list) {
      if(url.rfind(current->getUrl(),0) == 0) {
         return current;
      }
   }

   return null_handler; 
}

void doorctrl::HttpServer::handlePacketReceived(int32_t clientId, BaseLib::Http http) {

   doorctrl::HttpInterfaceLogger::getInstance().getOutput().printDebug("Debug: handle packet received");

   std::string payload;
   std::string header;
   BaseLib::TcpSocket::TcpPacket response;

   if(http.getHeader().method == "GET") {
      auto handler = this->getRequestHandler(http.getHeader().path);

      doorctrl::HttpInterfaceLogger::getInstance().getOutput().printDebug("Debug: http request path " +  http.getHeader().path);

      if(handler != nullptr) {
         handler->handleRequest(http, response);
      } else {
         header.append("HTTP/1.1 404 Not Found\r\n");
         header.append("Content-Type: text/plain\r\n");
         header.append("Content-Length: 0\r\n");
         response.insert(response.end(), header.begin(), header.end());
      }
   } else {
      header.append("HTTP/1.1 405 Method Not Allowed\r\n");
      header.append("Content-Type: text/plain\r\n");
      header.append("Content-Length: 0\r\n");
      response.insert(response.end(), header.begin(), header.end());
   }

   this->server->send(clientId, response);
}

void doorctrl::HttpServer::setup(std::shared_ptr<doorctrl::ConfigData>& config_data) {

   doorctrl::HttpInterfaceLogger::getInstance().getOutput().printDebug("Debug: setting up health http interface");
   this->config_data = config_data;

   this->server_info.packetReceivedCallback = std::bind(&doorctrl::HttpServer::handlePacketReceived, this, std::placeholders::_1, std::placeholders::_2);
   this->server_info.useSsl = false;
   this->server_info.connectionBacklogSize = 15;
   this->server_info.maxConnections = 5;
   this->server_info.serverThreads = 1;
   this->server_info.requireClientCert = false;
   this->server_info.dhParamFile = "";
   this->server_info.dhParamData = "";

   this->server = std::make_unique<BaseLib::HttpServer>(doorctrl::BaselibManager::getInstance().getSharedObjects().get(), this->server_info);

   std::string port = std::to_string(this->config_data->getHealthConfig().http_port);
   std::string bind_address = this->config_data->getHealthConfig().http_bind_address;

   doorctrl::HttpInterfaceLogger::getInstance().getOutput().printDebug("Debug: bind health interface to address " + this->config_data->getHealthConfig().http_bind_address + ", port = " + port);

   this->server->start("::", port, bind_address);

   doorctrl::HttpInterfaceLogger::getInstance().getOutput().printDebug("Debug: start listening for incoming requests");
}

