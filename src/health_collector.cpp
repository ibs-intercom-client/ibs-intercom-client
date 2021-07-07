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

#include "health_collector.h"
#include <mutex>
#include <chrono>
#include <functional>

static std::mutex mutex;
static const uint16_t MIN_SLEEP_DURATION = 15;

void doorctrl::HealthCollector::setup(std::shared_ptr<ConfigData>& config_data) {
   this->config_data = config_data;
   this->collector_thread = std::make_unique<std::thread>();
}


void doorctrl::HealthCollector::start() {
   this->collector_thread = 
     std::make_unique<std::thread>(std::bind(&doorctrl::HealthCollector::run_collector_loop, this));
}


bool doorctrl::HealthCollector::isHealthy() const {
  std::lock_guard<std::mutex> lock(mutex);
  return this->health_state;
}


void doorctrl::HealthCollector::run_collector_loop() {

   while(true) {

     uint16_t sleep_duration = this->config_data->getHealthConfig().health_collector_sleep;

     if(sleep_duration < MIN_SLEEP_DURATION) {
        sleep_duration = MIN_SLEEP_DURATION;
     }

     std::this_thread::sleep_for(std::chrono::seconds(sleep_duration));
   }
}
