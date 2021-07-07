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

#include "video_stream_state.h" 

std::string doorctrl::videoStreamStateToString(doorctrl::VideoStreamState state) {
  switch(state) {
    case VIDEO_STREAM_STATE_STARTING:
      return "VIDEO_STREAM_STATE_STARTING";
    case VIDEO_STREAM_STATE_RUNNING:
      return "VIDEO_STREAM_STATE_RUNNING";
    case VIDEO_STREAM_STATE_STOPPED:
      return "VIDEO_STREAM_STATE_STOPPED";
    case VIDEO_STREAM_STATE_ERROR:
      return "VIDEO_STREAM_STATE_ERROR";
    default:
      return "VIDEO_STREAM_STATE_UNKNOWN";
  }
}
