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

#ifndef __DOORCTRL_HTTP_INTERFACE_HTTP_REQUEST_HANDLER_H__
#define __DOORCTRL_HTTP_INTERFACE_HTTP_REQUEST_HANDLER_H__

#include <string>
#include <homegear-base/BaseLib.h>

namespace doorctrl {

   class HttpRequestHandler {
     public:
       virtual ~HttpRequestHandler() = default; 
       virtual const std::string& getUrl() const = 0;
       virtual void handleRequest(BaseLib::Http& http , BaseLib::TcpSocket::TcpPacket& response) const = 0;
   };
}


#endif
