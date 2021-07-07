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

#ifndef __DOORCTRL_HEALTH_COLLECTOR_H__
#define __DOORCTRL_HEALTH_COLLECTOR_H__

#include <thread>
#include <memory>
#include "common/health_provider.h"
#include "config/config_data.h"

namespace doorctrl {

   class HealthCollector : public HealthProvider {
      public:

        virtual ~HealthCollector() = default;
        void setup(std::shared_ptr<ConfigData>& config_data);
        void start();
        bool isHealthy() const override;
   
      private:
        void run_collector_loop();
        std::shared_ptr<ConfigData> config_data;
        std::unique_ptr<std::thread> collector_thread;
        bool health_state = true; 
   };
}

#endif
