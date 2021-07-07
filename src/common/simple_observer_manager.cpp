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

#include "simple_observer_manager.h"
#include "common_logger.h"

template<class T, class E>
std::mutex doorctrl::SimpleObserverManager<T, E>::observer_mutex;

template<class T, class E>
void doorctrl::SimpleObserverManager<T, E>::addObserver(const std::shared_ptr<T>& observer) {
  const std::lock_guard<std::mutex> lock(observer_mutex);

  this->observer_list.emplace(observer.get(), observer);

  doorctrl::CommonLogger::getInstance().getOutput().printDebug("Debug: adding observer to list, new count = " + std::to_string(this->observer_list.size()));
}

template<class T, class E>
void doorctrl::SimpleObserverManager<T, E>::removeObserver(const std::shared_ptr<T>& observer) {
  const std::lock_guard<std::mutex> lock(observer_mutex);
  this->observer_list.erase(observer.get());

  doorctrl::CommonLogger::getInstance().getOutput().printDebug("Debug: removing observer from list, new count = " + std::to_string(this->observer_list.size()));
}


template<class T, class E>
void doorctrl::SimpleObserverManager<T, E>::fireEvent(const E& event) {
  const std::lock_guard<std::mutex> lock(observer_mutex);

  doorctrl::CommonLogger::getInstance().getOutput().printDebug("Debug: firing event to all observers, count = " + std::to_string(this->observer_list.size()));

  for(auto item : this->observer_list) {
    item.second->handleEvent(event);
  }
}

