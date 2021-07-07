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

#ifndef __DOORCTRL_GTKUI_VIDEO_PANEL_H__
#define __DOORCTRL_GTKUI_VIDEO_PANEL_H__

#include <memory>
#include <thread>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include "config/config_data.h"
#include "video_stream_state.h"
#include "video_stream_state_observer.h"
#include "common/simple_observer_manager.h"

namespace doorctrl {

  class VideoPanel {
    public:
      explicit VideoPanel(std::shared_ptr<ConfigData>& config_data);
      virtual ~VideoPanel() = default;
      void setup();
      void setupVideoOverlay();
      void updateVideoStream();
      void enterStreamingState();
      void enterIdleState();
      GtkWidget* getHandle() const;
      void resizeImage(int width, int height);
      void setVideoPanelRealized(bool realized);
      bool isVideoPanelRealized() const;
      void addObserver(const std::shared_ptr<VideoStreamStateObserver>& observer);
      void removeObserver(const std::shared_ptr<VideoStreamStateObserver>& observer);


    private:
      void setupPipeline();
      void cleanupPipeline();

      std::shared_ptr<ConfigData> config_data;
      GtkWidget* layout_box = nullptr;
      GtkWidget* stack = nullptr;
      GtkWidget* image_widget = nullptr;
      GdkPixbuf* pixbuf = nullptr; 
      GtkWidget* upper_spacer = nullptr;
      GtkWidget* bottom_spacer = nullptr;
      GtkWidget* video_widget = nullptr;
      GtkWidget* video_box = nullptr;
      GstElement* pipeline = nullptr;
      GstElement* decoder = nullptr;
      GstElement* video_sink = nullptr;
      GstElement* video_source = nullptr;
      GstElement* demux = nullptr;
      GstElement* queue = nullptr;
      GstBus* gstreamer_bus;
      bool playing = false;
      bool video_panel_realized = false;
      int current_width = 0;
      int current_height = 0;
      std::unique_ptr<std::thread> realize_wait_thread; 
      SimpleObserverManager<VideoStreamStateObserver, VideoStreamStateEvent> video_observer_manager;
  };

}

#endif
