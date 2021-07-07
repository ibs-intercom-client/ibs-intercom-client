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

#include "pjsip_phone.h"

using namespace doorctrl;

int main(int argc, const char** argv) {

   Phone* phone = new PjsipPhone(); 
   phone->startup();

   doorctrl::Credentials creds = doorctrl::Credentials::createCredentials("tuer", "i0Krg3P");

   phone->registerClient("192.168.200.153", creds);

   // Here we don't have anything else to do..
   pj_thread_sleep(100000);

   phone->cleanup();
    
   return 0;
}
