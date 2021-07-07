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

#ifndef __DOORCTRL_ROBOT_ACCOUNT_H__
#define __DOORCTRL_ROBOT_ACCOUNT_H__

#include <pjsua2.hpp>
#include <string>
#include <memory>
#include "robot_call.h" 

namespace doorctrl {

  class RobotAccount : public pj::Account {
    public:
      RobotAccount();
      virtual ~RobotAccount();
      bool hasCall() const;
      void hangupCurrentCall();
      void makeCall();
      void onRegState(pj::OnRegStateParam& prm) override;

    private:
      void removeCurrentCall();
      std::unique_ptr<RobotCall> current_call;
  };
}

#endif
