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

#include "single_instance_lock.h"

#include <sys/types.h>
#include <sys/file.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


static const char* LOCK_FILE_PATH = "/var/run/doorbell/doorbell.pid";

bool doorctrl::SingleInstanceLock::lock() {

   this->fd = open(LOCK_FILE_PATH, O_CREAT | O_RDWR, 0644);

   if(this->fd < 0) {
      return false;
   }

   if(flock(this->fd, LOCK_EX | LOCK_NB) < 0) {
      this->fd = -1;
      return false;
   }

   int pid = getpid();
   FILE* f = fdopen(this->fd, "w");

   fprintf(f, "%d", pid);
   fflush(f);

   return true;
}


void doorctrl::SingleInstanceLock::unlock() {

   if(fd >= 0) {

      if(flock(this->fd, LOCK_UN) < 0) {
         return;
      }

      close(this->fd);

      if(unlink(LOCK_FILE_PATH) < 0) {

      }

      this->fd = -1;
   }
}

