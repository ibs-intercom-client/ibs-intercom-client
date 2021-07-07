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

#ifndef _DOORCTRL_HOMEGEAR_CONNECTOR_HOMEGEAR_CONFIG_DATA_CONNECTOR_H__
#define _DOORCTRL_HOMEGEAR_CONNECTOR_HOMEGEAR_CONFIG_DATA_CONNECTOR_H__

#include <string>
#include <memory>
#include <mutex>
#include <thread>
#include <inttypes.h>
#include "rpc_client.h"
#include "config/config_data.h"
#include "config/config_data_persistence.h"

namespace doorctrl {

   class IntercomConfigDataConnector : public ConfigDataPersistence {
     public:
       void setup(const std::shared_ptr<RpcClient>& client, const std::shared_ptr<ConfigData>& config_data);
       void startIntercomConfigMonitor();
       void readRemoteConfigData();
       void saveConfigData(const std::shared_ptr<ConfigData>& config_data) override;
       void startSingleConfigDataRead();
       void doSingleConfigDataRead();

     private:
       BaseLib::PVariable readDeviceVariable(const std::string& variable_name);
       void writeDeviceVariable(const std::string& variable_name, BaseLib::PVariable value);
       int32_t searchIntercomDevice();
       void updateIntercomDevice();
       void runIntercomConfigMonitorLoop();
       void runSingleConfigDataRead();

       std::shared_ptr<RpcClient> client;
       std::shared_ptr<ConfigData> config_data;
       int32_t intercom_device_id = -1;
       std::unique_ptr<std::thread> monitor_thread = nullptr;
       std::unique_ptr<std::thread> single_read_thread = nullptr;
       std::mutex config_data_mutex;
       std::mutex single_read_mutex;
   };
}

#endif
