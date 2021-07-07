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

#ifndef __DOORCTRL_COMMON_INSTALLATION_H__
#define __DOORCTRL_COMMON_INSTALLATION_H__

#include "config/config_data.h" 
#include <memory>

namespace doorctrl {

   class Installation {
      public:
        virtual ~Installation() {}
        virtual void setup(std::shared_ptr<ConfigData>& config_data) = 0;
        virtual void cleanup() = 0;
        virtual void enterInitialState() = 0;
        virtual const std::string& getRegisteredUserName() const = 0;

      private:
        std::unique_ptr<Phone> phone;  
        std::unique_ptr<DoorOpener> door_opener;  
        std::unique_ptr<RingtonePlayer> ringtone_player;  
   };

}

#endif
