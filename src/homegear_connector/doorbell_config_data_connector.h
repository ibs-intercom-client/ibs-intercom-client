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

#ifndef _DOORCTRL_HOMEGEAR_CONNECTOR_DOORBELL_CONFIG_DATA_CONNECTOR_H__
#define _DOORCTRL_HOMEGEAR_CONNECTOR_DOORBELL_CONFIG_DATA_CONNECTOR_H__

#include <string>
#include <memory>
#include <mutex>
#include <thread>
#include <inttypes.h>
#include "rpc_client.h"
#include "config/config_data.h"
#include "config/config_data_persistence.h"
#include "common/push_button_state_observer.h"

namespace doorctrl {

   class DoorbellConfigDataConnector : public PushButtonStateObserver {
     public:
       void setup(const std::shared_ptr<RpcClient>& client, const std::shared_ptr<ConfigData>& config_data);
       void startDoorbellConfigMonitor();
       void readRemoteConfigData();
       void handleEvent(const PushButtonStateEvent& event) override;
       void doSingleConfigDataRead();

     private:
       BaseLib::PVariable readDeviceVariable(const std::string& variable_name);
       void writeDeviceVariable(const std::string& variable_name, BaseLib::PVariable value);
       int32_t searchDoorbellDevice();
       void updateDoorbellDevice();
       void runDoorbellConfigMonitorLoop();

       std::shared_ptr<RpcClient> client;
       std::shared_ptr<ConfigData> config_data;
       int32_t doorbell_device_id = -1;
       std::unique_ptr<std::thread> monitor_thread;
       std::mutex config_data_mutex;
   };
}

#endif
