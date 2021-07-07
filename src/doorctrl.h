/*
 * Copyright (C) 2021 Homegear GmbH 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either version 3 of the 
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, see  <http://www.gnu.org/licenses/>.
 */

#ifndef __DOORCTRL_DOORCTRL_H__
#define __DOORCTRL_DOORCTRL_H__

#include <memory>
#include "common/phone.h"
#include "common/door_opener.h"
#include "common/ringtone_player.h"
#include "common/phone_call_state_observer.h"
#include "common/phone_registration_state_observer.h"
#include "common/installation.h"
#include "config/config_data.h"
#include "http_interface/http_server.h"
#include "health_collector.h"
#include "reboot_handler.h"
#include "homegear_connector/rpc_client.h"
#include "homegear_connector/intercom_config_data_connector.h"
#include "homegear_connector/rpc_event_connector.h"
#include "gpio_connector/home_button_connector.h"

namespace doorctrl {

  class Doorctrl : public Installation, 
                   public PhoneCallStateObserver,
                   public PhoneRegistrationStateObserver,
                   public std::enable_shared_from_this<Doorctrl> {

    public:
      void setup(std::shared_ptr<ConfigData>& config_data) override;
      void cleanup() override;
      const std::string& getRegisteredUserName() const override;
      void handleEvent(const PhoneCallStateEvent& event) override;
      void handleEvent(const PhoneRegistrationStateEvent& event) override;

      std::shared_ptr<Phone>& getPhone();
      std::shared_ptr<DoorOpener>& getDoorOpener();
      std::shared_ptr<HomeButton>& getHomeButton();
      std::shared_ptr<ConfigData>& getConfigData();
      static void printStartupMessage();

    private:

      void enterInitialState() override;
      void enterIdleState();
      void setupHomegearRpcClient();
      void setupIntercomConfigDataConnector();
      void setupDoorctrlHealth();

      std::shared_ptr<Phone> phone;
      std::shared_ptr<DoorOpener> door_opener;
      std::shared_ptr<RingtonePlayer> ringtone_player;
      std::shared_ptr<ConfigData> config_data;
      std::shared_ptr<HttpServer> health_interface;
      std::shared_ptr<HealthCollector> health_collector;
      std::shared_ptr<RebootHandler> reboot_handler;
      std::shared_ptr<RpcEventConnector> rpc_event_connector;
      std::shared_ptr<RpcClient> rpc_client;
      std::shared_ptr<IntercomConfigDataConnector> config_data_connector;
      std::shared_ptr<HomeButton> home_button;
  };
}

#endif
