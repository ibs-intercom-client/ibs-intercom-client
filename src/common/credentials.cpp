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

#include "credentials.h"


doorctrl::Credentials doorctrl::Credentials::createCredentials(const std::string& username, const std::string& password) {

   doorctrl::Credentials cred;

   cred.username = username;
   cred.password = password;

   return cred;
}


bool doorctrl::Credentials::equals(const Credentials& other) const {

  if(this->username.compare(other.username) != 0) return false;

  if(this->password.compare(other.password) != 0) return false;

  return true;
}
