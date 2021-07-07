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

#include "baselib_manager.h"

doorctrl::BaselibManager::BaselibManager()
 : bl(new BaseLib::SharedObjects(false))  {

}

doorctrl::BaselibManager& doorctrl::BaselibManager::getInstance() {
   static BaselibManager instance;
   return instance;
}

std::shared_ptr<BaseLib::SharedObjects> doorctrl::BaselibManager::getSharedObjects() {
  return this->bl;
}

