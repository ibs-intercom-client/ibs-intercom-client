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

#ifndef __DOORCTRL_GPIO_CONNECTOR_PUSH_BUTTON_CONNECTOR_H__
#define __DOORCTRL_GPIO_CONNECTOR_PUSH_BUTTON_CONNECTOR_H__

#include <thread>
#include <memory>
#include <gpiod.h>
#include "common/health_provider.h"
#include "common/push_button_state_observer.h"
#include "common/simple_observer_manager.h"
#include "config/config_data.h"
#include <homegear-base/BaseLib.h>

namespace doorctrl {

   class PushButtonConnector {
      public:

        virtual ~PushButtonConnector();
        void setup(std::shared_ptr<ConfigData>& config_data);
        void start();
        bool isPressed() const;
        void addObserver(const std::shared_ptr<PushButtonStateObserver>& observer);
        void removeObserver(const std::shared_ptr<PushButtonStateObserver>& observer);
   
      private:
        void run_polling_loop();
        std::shared_ptr<ConfigData> config_data;
        std::shared_ptr<BaseLib::LowLevel::Gpio> gpio_wrapper;
        std::unique_ptr<std::thread> polling_thread;
        bool pressed  = false; 
        std::unique_ptr<BaseLib::SharedObjects> bl;
        SimpleObserverManager<PushButtonStateObserver, PushButtonStateEvent> state_observer_manager;
   };
}

#endif
