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

#ifndef __DOORCTRL_GSTREAMER_MEDIA_GSTREAMER_MEDIA_LOGGER_H__
#define __DOORCTRL_GSTREAMER_MEDIA_GSTREAMER_MEDIA_LOGGER_H__


#include <homegear-base/Output/Output.h>

namespace doorctrl {
  
  class GStreamerMediaLogger {
    public:
      /*
       * remove default copy constructor and copy operator
       */
      GStreamerMediaLogger(const GStreamerMediaLogger& logger) = delete;
      GStreamerMediaLogger& operator=(const GStreamerMediaLogger& logger) = delete;
      BaseLib::Output& getOutput();
      static GStreamerMediaLogger& getInstance();

    private:
      GStreamerMediaLogger();
      BaseLib::Output output; 
  };
}

#endif
