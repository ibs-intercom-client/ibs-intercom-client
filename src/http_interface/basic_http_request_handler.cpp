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

#include "basic_http_request_handler.h"


doorctrl::BasicHttpRequestHandler::BasicHttpRequestHandler(const std::string& url)
  : url(url) {

}


const std::string& doorctrl::BasicHttpRequestHandler::getUrl() const {
   return this->url;
}

void doorctrl::BasicHttpRequestHandler::createInvalidMethodError(BaseLib::Http& http , BaseLib::TcpSocket::TcpPacket& response) const {
  std::string header;
  header.append("HTTP/1.1 405 Method Not Allowed\r\n");
  header.append("Content-Type: text/plain\r\n");
  header.append("Content-Length: 0\r\n");
  response.insert(response.end(), header.begin(), header.end());
}
