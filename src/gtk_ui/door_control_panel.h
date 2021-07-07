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

#ifndef __DOORCTRL_GTKUI_DOOR_CONTROL_PANEL_H__
#define __DOORCTRL_GTKUI_DOOR_CONTROL_PANEL_H__

#include <gtk/gtk.h>
#include <memory>
#include <atomic>
#include "door_control_button_monkey.h" 

namespace doorctrl {

  class UiControler;

  class DoorControlPanel {
    public:

      void setup(std::shared_ptr<UiControler> &controler);
      GtkWidget *getHandle() const;
      void enterRingingState();
      void enterIdleState();
      void enterAnsweringState();
      void enterConnectingState();
      void enterVideoStreamErrorState();
      void enterVideoStreamNormalState();
      void updateHighlightColor(const std::string& highlight_color);

    private:
      void setupNameLabel();
      void setupCenterBox();
      void setupStateIndicator();
      void setupButtons();
      void setupActivityButton();
      void setupHangupButton();
      void setupMuteButton();
      void setupVolumeButton();
      void setupMediaBox();
      void setupButtonMonkey();
      void changeButtonText(GtkWidget* button, const gchar* text);
      void openSpeakerVolumePopover(); 
      void incrementSpeakerVolume(); 
      void decrementSpeakerVolume(); 
      void doMuteMic(bool mute);

      bool volumePopoverVisible = false;

      std::shared_ptr<UiControler> controler;
      GtkBox* layout_box = nullptr;
      GtkBox* phone_button_box = nullptr;
      GtkBox* volume_box = nullptr;
      GtkBox* center_box = nullptr;
      GtkBox* media_box = nullptr;
      GtkWidget* volume_popover = nullptr;
      GtkWidget* activity_button = nullptr;
      GtkWidget* hangup_button = nullptr;
      GtkWidget* volume_button = nullptr;
      GtkWidget* volume_up_button = nullptr;
      GtkWidget* volume_down_button = nullptr;
      GtkWidget* volume_value_label = nullptr;
      GtkWidget* mute_button = nullptr;
      GtkWidget* state_indicator = nullptr;
      GtkWidget* ready = nullptr;
      GtkWidget* ringing = nullptr;
      GtkWidget* established = nullptr;
      GtkWidget* connecting = nullptr;
      GtkWidget* mute_mic_icon = nullptr;
      GtkWidget* mic_icon = nullptr;
      GtkWidget* unmute_icon = nullptr;
      GtkWidget* volume_icon = nullptr;
      GtkWidget* volume_slider = nullptr;
      GtkWidget* error_subline = nullptr;
      GdkPixbuf* mute_mic_buf = nullptr;
      GdkPixbuf* mic_buf = nullptr;
      GtkCssProvider* state_label_style_provider = nullptr;
      GtkCssProvider* activity_button_style_provider = nullptr;
      GtkCssProvider* volume_button_style_provider = nullptr;
      GtkCssProvider* mute_button_style_provider = nullptr;
      GtkCssProvider* hangup_button_style_provider = nullptr;
      gulong activity_signal_id = 0;
      std::atomic<bool> has_video_stream_error;
      std::unique_ptr<DoorControlButtonMonkey> monkey;
      static void handleVolumeButtonEvent(GtkButton* button, gpointer user_data);
      static void handleVolumeUpButtonEvent(GtkButton* button, gpointer user_data);
      static void handleVolumeDownButtonEvent(GtkButton* button, gpointer user_data);
      static void handleMuteButtonEvent(GtkToggleButton* button, gpointer user_data);
  };
}

#endif
