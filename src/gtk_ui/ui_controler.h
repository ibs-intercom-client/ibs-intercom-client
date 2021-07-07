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

#ifndef __DOORCTRL_GTK_UI_UI_CONTROLER_H__
#define __DOORCTRL_GTK_UI_UI_CONTROLER_H__

#include <gtk/gtk.h>
#include <memory>
#include <deque>
#include <functional>
#include <mutex>
#include "common/phone.h"
#include "common/phone_call_state.h"
#include "common/phone_call_state_observer.h"
#include "video_stream_state.h"
#include "video_stream_state_observer.h"
#include "common/phone_registration_state_observer.h"
#include "common/door_opener.h"
#include "door_control_panel.h"
#include "config/config_data.h"
#include "config/config_data_changed_observer.h"
#include "video_panel.h"
#include "common/home_button.h"
#include "common/home_button_state_observer.h"

namespace doorctrl {

  class UiControler : public PhoneCallStateObserver, 
                      public PhoneRegistrationStateObserver,
                      public HomeButtonStateObserver,
                      public VideoStreamStateObserver, 
                      public ConfigDataChangedObserver, 
                      public std::enable_shared_from_this<UiControler> {

    public:
      /*
       * remove default copy constructor and copy operator
       */
      UiControler(const UiControler& controler) = delete;
      UiControler& operator=(const UiControler& controler) = delete;

      explicit UiControler(std::shared_ptr<Phone>& phone, 
                           std::shared_ptr<DoorOpener>& opener,
                           std::shared_ptr<HomeButton>& home_button);

      virtual ~UiControler();
      void setup(std::shared_ptr<ConfigData>& config_data);
      void enterInitialState();
      void updateHighlightColor(); 
      void setDoorControlPanel(std::shared_ptr<DoorControlPanel>& panel);
      void setVideoPanel(std::shared_ptr<VideoPanel>& panel);
      bool hasDoorOpener() const;
      void doHangup();
      void doAnswer();
      void doOpenVideo();
      void doDoorOpener();
      void doMuteMic(bool mute);
      bool isMicMuted() const;
      void setSpeakerVolume(uint8_t volume);
      uint8_t getSpeakerVolume() const;
      const std::string& getRegisteredUserName() const;
      void handleEvent(const PhoneCallStateEvent& event) override;
      void handleEvent(const PhoneRegistrationStateEvent& event) override;
      void handleEvent(const VideoStreamStateEvent& event) override;
      void handleEvent(const ConfigDataChangedEvent& event) override;
      void handleEvent(const HomeButtonStateEvent& event) override;
      void handleQueuedPhoneCallStateEvents();
      void handleQueuedPhoneRegistrationStateEvents();
      void handleQueuedVideoStreamStateEvents();
      void handleQueuedHomeButtonStateEvents();

      static void handleHangupButtonEvent(GtkButton* button, gpointer user_data);
      static void handleAnswerButtonEvent(GtkButton* button, gpointer user_data);
      static void handleDoorOpenerButtonEvent(GtkButton* button, gpointer user_data);
      static void handleSpeakerVolumeChangedEvent(GtkScaleButton* button, double value, gpointer user_data);

    private:
      void enterIdleState();
      void enterRingingState();
      void enterEstablishedState();
      void handleEventSync(const PhoneCallStateEvent& event);
      void handleEventSync(const PhoneRegistrationStateEvent& event);
      void handleEventSync(const VideoStreamStateEvent& event);
      void handleEventSync(const HomeButtonStateEvent& event);

      std::shared_ptr<Phone> phone; 
      std::shared_ptr<DoorOpener> door_opener;
      std::shared_ptr<HomeButton> home_button;
      std::shared_ptr<DoorControlPanel> door_control_panel;
      std::shared_ptr<VideoPanel> video_panel;
      std::shared_ptr<ConfigData> config_data;
      GtkWidget* the_window = nullptr;
      PhoneCallState state = PHONE_CALL_STATE_IDLE;
      std::string last_active_window_title = "";
      std::deque<PhoneCallStateEvent> phone_call_state_event_queue;
      std::deque<PhoneRegistrationStateEvent> phone_registration_state_event_queue;
      std::deque<VideoStreamStateEvent> video_stream_state_event_queue;
      std::deque<HomeButtonStateEvent> home_button_state_event_queue;
      std::mutex queue_mutex;
  };
}

#endif
