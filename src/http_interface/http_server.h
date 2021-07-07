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

#ifndef __DOORCTRL_HTTP_INTERFACE_HTTP_SERVER_H__
#define __DOORCTRL_HTTP_INTERFACE_HTTP_SERVER_H__

#include <memory>
#include <homegear-base/BaseLib.h>
#include <list>
#include "config/config_data.h"
#include "http_interface_logger.h"
#include "http_request_handler_manager.h"

namespace doorctrl {

   class HttpServer : public HttpRequestHandlerManager,
                      public std::enable_shared_from_this<HttpServer> {
     public:
        void setup(std::shared_ptr<ConfigData>& config_data);
        void cleanup();

        void addRequestHandler(const std::shared_ptr<HttpRequestHandler>& handler) override;
        std::shared_ptr<HttpRequestHandler> getRequestHandler(const std::string& url) const override;

     private:
        void handlePacketReceived(int32_t clientId, BaseLib::Http http);

        std::shared_ptr<ConfigData> config_data;
        std::list<std::shared_ptr<HttpRequestHandler> > request_handler_list;
        std::unique_ptr<BaseLib::HttpServer> server;
        BaseLib::HttpServer::HttpServerInfo server_info;
   };
}

#endif
