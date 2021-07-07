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

#ifndef __DOORCTRL_COMMON_SIMPLE_OBSERVER_MANAGER_H__
#define __DOORCTRL_COMMON_SIMPLE_OBSERVER_MANAGER_H__

#include <memory>
#include <mutex>
#include <unordered_map>

namespace doorctrl {

  template<class T, class E>
  class SimpleObserverManager {
    public:
      virtual ~SimpleObserverManager() = default;

      void addObserver(const std::shared_ptr<T>& observer);
      void removeObserver(const std::shared_ptr<T>& observer);
      void fireEvent(const E& event);

    private:
      std::unordered_map<T*, std::shared_ptr<T> > observer_list; 
      static std::mutex observer_mutex;
  };
}

#endif
