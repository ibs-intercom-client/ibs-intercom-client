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

#ifndef __DOORCTRL_GSTREAMER_MEDIA_GSTREAMER_RINGTONE_PLAYER_H__
#define __DOORCTRL_GSTREAMER_MEDIA_GSTREAMER_RINGTONE_PLAYER_H__

#include <gst/gst.h>
#include <memory>
#include <mutex>
#include <thread>
#include "common/ringtone_player.h"
#include "config/config_data.h"
#include "config/config_data_changed_observer.h"

namespace doorctrl {

  class GStreamerRingtonePlayer : public RingtonePlayer,
           public ConfigDataChangedObserver,
           public std::enable_shared_from_this<GStreamerRingtonePlayer>  {
     public:
        explicit GStreamerRingtonePlayer(std::shared_ptr<ConfigData>& config_data);
        ~GStreamerRingtonePlayer();
        void setup() override;
        void cleanup() override;
        void startRinging() override;
        void ringOnce() override;
        void stopRinging() override;
        void setVolume(uint8_t volume) override;
        uint8_t getVolume() override;
        void setMute(bool mute) override;
        bool getMute() override;
        void handleEvent(const ConfigDataChangedEvent& event) override; 

     private:
        void setupPipeline(bool loop = true);
        void cleanupPipeline();
        void handleRingOnce();

        std::shared_ptr<ConfigData> config_data; 
        GstElement* pipeline = nullptr;
        uint8_t volume = 100;
        GstBus* gstreamer_bus = nullptr;
        GstElement* volume_element = nullptr;
        GstElement* filesrc = nullptr;
        bool mute = false;
        std::mutex player_mutex;
        bool playing = false;
        std::unique_ptr<std::thread> ring_once_thread;
  };

} 

#endif
