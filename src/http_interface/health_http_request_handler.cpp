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

#include "health_http_request_handler.h"
#include "http_interface_logger.h"

doorctrl::HealthHttpRequestHandler::HealthHttpRequestHandler(const std::string& url, 
                                                             const std::shared_ptr<HealthProvider>& health_provider) 
  : BasicHttpRequestHandler(url), health_provider(health_provider) {

}


void doorctrl::HealthHttpRequestHandler::handleRequest(BaseLib::Http& http , BaseLib::TcpSocket::TcpPacket& response) const {

   if(http.getHeader().method == "GET") {
      if(this->health_provider != nullptr && this->health_provider->isHealthy()) {
         std::string header;
         header.append("HTTP/1.1 200 Ok\r\n");
         header.append("Content-Type: text/plain\r\n");
         header.append("Content-Length: 4\r\n");
         std::string payload = "OK\r\n";
         response.insert(response.end(), header.begin(), header.end());
         response.insert(response.end(), payload.begin(), payload.end());
      } else {
         std::string header;
         header.append("HTTP/1.1 500 Internal Server Error\r\n");
         header.append("Content-Type: text/plain\r\n");
         header.append("Content-Length: 11\r\n");
         std::string payload = "UNHEALTHY\r\n";
         response.insert(response.end(), header.begin(), header.end());
         response.insert(response.end(), payload.begin(), payload.end());
      }
   } else {
      createInvalidMethodError(http, response);
   }
}
