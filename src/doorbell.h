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

#ifndef __DOORCTRL_DOORBELL_H__
#define __DOORCTRL_DOORBELL_H__

#include <memory>
#include "common/ringtone_player.h"
#include "common/push_button_state_observer.h"
#include "config/config_data.h"
#include "health_collector.h"
#include "homegear_connector/rpc_client.h"
#include "homegear_connector/rpc_event_connector.h"
#include "homegear_connector/doorbell_config_data_connector.h"
#include "gpio_connector/push_button_connector.h"

namespace doorctrl {

  class Doorbell : public PushButtonStateObserver, public std::enable_shared_from_this<Doorbell> {

    public:
      void setup(std::shared_ptr<ConfigData>& config_data);
      void cleanup();

      std::shared_ptr<ConfigData>& getConfigData();
      void handleEvent(const PushButtonStateEvent& event) override;
      static void printStartupMessage();
    

    private:

      void setupHomegearRpcClient();
      void setupDoorbellConfigDataConnector();
      void setupDoorbellHealth();

      std::shared_ptr<RingtonePlayer> ringtone_player;
      std::shared_ptr<ConfigData> config_data;
      std::shared_ptr<HealthCollector> health_collector;
      std::shared_ptr<RpcEventConnector> rpc_event_connector;
      std::shared_ptr<RpcClient> rpc_client;
      std::shared_ptr<PushButtonConnector> push_button_connector;
      std::shared_ptr<DoorbellConfigDataConnector> config_data_connector;
  };
}

#endif
