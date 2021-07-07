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

#ifndef __DOORCTRL_GPIO_CONNECTOR_HOME_BUTTON_CONNECTOR_H__
#define __DOORCTRL_GPIO_CONNECTOR_HOME_BUTTON_CONNECTOR_H__

#include <thread>
#include <memory>
#include <gpiod.h>
#include "common/health_provider.h"
#include "common/home_button_state_observer.h"
#include "common/simple_observer_manager.h"
#include "config/config_data.h"
#include <homegear-base/BaseLib.h>
#include "common/home_button.h"

namespace doorctrl {

   class HomeButtonConnector : public HomeButton {
      public:

        virtual ~HomeButtonConnector();
        void setup(std::shared_ptr<ConfigData>& config_data);
        void start();
        bool isPressed() const;
        void addObserver(const std::shared_ptr<HomeButtonStateObserver>& observer);
        void removeObserver(const std::shared_ptr<HomeButtonStateObserver>& observer);
   
      private:
        void run_polling_loop();
        std::shared_ptr<ConfigData> config_data;
        std::shared_ptr<BaseLib::LowLevel::Gpio> gpio_wrapper;
        std::unique_ptr<std::thread> polling_thread;
        bool pressed  = false; 
        std::unique_ptr<BaseLib::SharedObjects> bl;
        SimpleObserverManager<HomeButtonStateObserver, HomeButtonStateEvent> state_observer_manager;
   };
}

#endif
