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

#include "ui_controler.h"
#include "door_control_panel.h"
#include "video_panel.h"
#include "gtk_ui_logger.h"
#include "x_window_controler.h"


/*
 * this call back function is executed in the UI main thread 
 * to allow UI modifications. Changing UI elements directly from
 * a thread other than the main thread will result in strange or bad
 * UI behavior (graphical artifacts, missing elements, deadlocks). 
 */
gboolean handle_queued_phone_call_state_events(gpointer user_data) {
   doorctrl::UiControler* controler = static_cast<doorctrl::UiControler*>(user_data);

   if(controler != nullptr) {
     controler->handleQueuedPhoneCallStateEvents();
   }

   return true;
}


/*
 * this call back function is executed in the UI main thread 
 * to allow UI modifications. Changing UI elements directly from
 * a thread other than the main thread will result in strange or bad
 * UI behavior (graphical artifacts, missing elements, deadlocks). 
 */
gboolean handle_queued_phone_registration_state_events(gpointer user_data) {
   doorctrl::UiControler* controler = static_cast<doorctrl::UiControler*>(user_data);

   if(controler != nullptr) {
      controler->handleQueuedPhoneRegistrationStateEvents();
   }


   return true;
}


/*
 * this call back function is executed in the UI main thread 
 * to allow UI modifications. Changing UI elements directly from
 * a thread other than the main thread will result in strange or bad
 * UI behavior (graphical artifacts, missing elements, deadlocks). 
 */
gboolean handle_queued_video_stream_events(gpointer user_data) {
   doorctrl::UiControler* controler = static_cast<doorctrl::UiControler*>(user_data);

   if(controler != nullptr) {
      controler->handleQueuedVideoStreamStateEvents();
   }

   return true;
}

gboolean handle_queued_home_button_state_events(gpointer user_data) {
   doorctrl::UiControler* controler = static_cast<doorctrl::UiControler*>(user_data);

   if(controler != nullptr) {
      controler->handleQueuedHomeButtonStateEvents();
   }

   return true;
}


doorctrl::UiControler::UiControler(std::shared_ptr<Phone>& phone,
                                   std::shared_ptr<DoorOpener>& opener,
                                   std::shared_ptr<HomeButton>& home_button)
  : phone(phone), door_opener(opener), home_button(home_button), state(PHONE_CALL_STATE_IDLE) {

}


doorctrl::UiControler::~UiControler() {
  (static_cast<doorctrl::ObservedSubject<doorctrl::PhoneCallStateObserver>*>(this->phone.get()))->removeObserver(shared_from_this());
  (static_cast<doorctrl::ObservedSubject<doorctrl::PhoneRegistrationStateObserver>*>(this->phone.get()))->removeObserver(shared_from_this());
  this->home_button->removeObserver(shared_from_this());
}


void doorctrl::UiControler::setup(std::shared_ptr<doorctrl::ConfigData>& config_data) {
  this->config_data = config_data;
  (static_cast<doorctrl::ObservedSubject<doorctrl::PhoneCallStateObserver>*>(this->phone.get()))->addObserver(shared_from_this());
  (static_cast<doorctrl::ObservedSubject<doorctrl::PhoneRegistrationStateObserver>*>(this->phone.get()))->addObserver(shared_from_this());

  this->home_button->addObserver(shared_from_this());

  UiConfig ui_config = this->config_data->getUiConfig();

  if(this->door_control_panel != nullptr) {
     this->door_control_panel->updateHighlightColor(ui_config.highlight_color);
  }

  /*
   * Register callback functions to handle events in the main thread.
   * The events have been written to event queues to do so
   */
  gdk_threads_add_timeout(100, &handle_queued_phone_call_state_events, this); 
  gdk_threads_add_timeout(100, &handle_queued_phone_registration_state_events, this); 
  gdk_threads_add_timeout(100, &handle_queued_video_stream_events, this);
  gdk_threads_add_timeout(100, &handle_queued_home_button_state_events, this);

  XWindowControler::setup();
}


void doorctrl::UiControler::enterInitialState() {
   
   if(this->phone->hasIncomingCall()) {
     this->enterRingingState();
   } else {
     if(this->phone->isRegistering()) {
        if(this->door_control_panel != nullptr) {
           this->door_control_panel->enterConnectingState(); 
        }
     } else {
        this->enterIdleState();
     }
   } 
}


void doorctrl::UiControler::enterIdleState() {

  if(this->door_control_panel != nullptr) {
     this->door_control_panel->enterIdleState();
  }

  if(this->video_panel != nullptr) {
     this->video_panel->enterIdleState();
  }

  this->state = PHONE_CALL_STATE_IDLE;
}


void doorctrl::UiControler::setDoorControlPanel(std::shared_ptr<DoorControlPanel>& panel) {
   this->door_control_panel = panel;
}


void doorctrl::UiControler::setVideoPanel(std::shared_ptr<VideoPanel>& panel) {
   this->video_panel = panel;
}


bool doorctrl::UiControler::hasDoorOpener() const {
   return (this->door_opener != nullptr);
} 

void doorctrl::UiControler::doHangup() {
   this->phone->hangup();

   this->enterIdleState();
}


void doorctrl::UiControler::doAnswer() {
   this->phone->answer();
}


void doorctrl::UiControler::doOpenVideo() {

}


void doorctrl::UiControler::doDoorOpener() {
   if(this->door_opener != nullptr) {
      this->door_opener->buzz();
   }
}


void doorctrl::UiControler::doMuteMic(bool mute) {

  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: change microphone mute, state = " + std::to_string(mute));

  this->phone->muteMicrophone(mute);
}


bool doorctrl::UiControler::isMicMuted() const {

   return false;
}


void doorctrl::UiControler::setSpeakerVolume(uint8_t volume) {
  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: set speaker volume = " + std::to_string((uint16_t)volume));
  this->phone->setSpeakerVolume(volume);

  PhoneConfig phone_config = this->config_data->getPhoneConfig();
  phone_config.volume = volume;
  this->config_data->updatePhoneConfig(phone_config);
  this->config_data->save();
}


uint8_t doorctrl::UiControler::getSpeakerVolume() const {
   return this->phone->getSpeakerVolume();
}


const std::string& doorctrl::UiControler::getRegisteredUserName() const {
   return phone->getRegisteredUserName();
} 


void doorctrl::UiControler::handleEvent(const doorctrl::PhoneCallStateEvent& event) {

  std::lock_guard<std::mutex> lock(this->queue_mutex);
  this->phone_call_state_event_queue.push_back(event);

}


void doorctrl::UiControler::handleEvent(const doorctrl::VideoStreamStateEvent& event) {

  std::lock_guard<std::mutex> lock(this->queue_mutex);
  this->video_stream_state_event_queue.push_back(event);

}


void doorctrl::UiControler::handleEvent(const doorctrl::PhoneRegistrationStateEvent& event) {

  std::lock_guard<std::mutex> lock(this->queue_mutex);
  this->phone_registration_state_event_queue.push_back(event);

}


void doorctrl::UiControler::handleEvent(const doorctrl::ConfigDataChangedEvent& event) {

   if (event.getChangedDataSection().compare(ConfigDataChangedEvent::UI_SECTION_NAME) == 0) {
      this->updateHighlightColor();
   }
}


void doorctrl::UiControler::handleEvent(const doorctrl::HomeButtonStateEvent& event) {
   std::lock_guard<std::mutex> lock(this->queue_mutex);

   this->home_button_state_event_queue.push_back(event);
}


void doorctrl::UiControler::updateHighlightColor() {

  UiConfig ui_config = this->config_data->getUiConfig();

  if(this->door_control_panel != nullptr) {
     this->door_control_panel->updateHighlightColor(ui_config.highlight_color);
  }
}


void doorctrl::UiControler::handleEventSync(const doorctrl::PhoneRegistrationStateEvent& event) {

  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: handling phone registration state change, registration state =  " + doorctrl::PhoneRegistrationStateToString(event.getCurrentRegistrationState()));

   if(event.getCurrentRegistrationState() == doorctrl::PHONE_REGISTRATION_STATE_IN_PROGRESS) {
     if(this->door_control_panel != nullptr) {
       this->door_control_panel->enterConnectingState(); 
     }
   } else if(event.getCurrentRegistrationState() == doorctrl::PHONE_REGISTRATION_STATE_REGISTERED) {
     if(this->door_control_panel != nullptr) {
       this->door_control_panel->enterIdleState(); 
     }
   }
}


void doorctrl::UiControler::handleEventSync(const doorctrl::PhoneCallStateEvent& event) {

  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: handling phone call state change, current state =  " + doorctrl::phoneCallStateToString(event.getCurrentCallState()));

  switch(event.getCurrentCallState()) {
    case PHONE_CALL_STATE_RINGING:
      this->enterRingingState(); 
      break;
    case PHONE_CALL_STATE_ESTABLISHED:
      this->enterEstablishedState();
      break;
    case PHONE_CALL_STATE_FINISHED:
    case PHONE_CALL_STATE_TERMINATED:
      this->enterIdleState();

      doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: call finished - last active window title = " + this->last_active_window_title);

      if(!this->last_active_window_title.empty()) {
         doorctrl::XWindowControler::bringToFront(this->last_active_window_title);
      }
      doorctrl::XWindowControler::enableScreensaver();

      break;
    default:
      this->enterIdleState();
      doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: unknown phone event - last active window title = " + this->last_active_window_title);
      if(!this->last_active_window_title.empty()) {
         doorctrl::XWindowControler::bringToFront(this->last_active_window_title);
      }
  }
}


void doorctrl::UiControler::handleEventSync(const doorctrl::VideoStreamStateEvent& event) {
    
  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: handle video stream event, state = " + doorctrl::videoStreamStateToString(event.getVideoStreamState()));

  if(event.getVideoStreamState() == VIDEO_STREAM_STATE_ERROR) {
    if(this->door_control_panel != nullptr) {
      this->door_control_panel->enterVideoStreamErrorState(); 
    }
  } else {
    if(this->door_control_panel != nullptr) {
      this->door_control_panel->enterVideoStreamNormalState(); 
    }
  }
}


void doorctrl::UiControler::handleEventSync(const doorctrl::HomeButtonStateEvent& event) {

   doorctrl::XWindowControler::wakeupScreen();

   if(!this->phone->hasIncomingCall() && !this->phone->hasEstablishedCall()) { 
      if(!event.isPressed()) {
         doorctrl::XWindowControler::bringToFront("Chromium");
      }
   }
}


void doorctrl::UiControler::handleQueuedPhoneRegistrationStateEvents() {

   std::unique_lock<std::mutex> lock(this->queue_mutex);

   while(this->phone_registration_state_event_queue.size() > 0) {
      doorctrl::PhoneRegistrationStateEvent event = this->phone_registration_state_event_queue.front();
      this->phone_registration_state_event_queue.pop_front();
    
      lock.unlock();
      this->handleEventSync(event);
      lock.lock();
   }
}


void doorctrl::UiControler::handleQueuedPhoneCallStateEvents() {

   std::unique_lock<std::mutex> lock(this->queue_mutex);

   while(this->phone_call_state_event_queue.size() > 0) {
      doorctrl::PhoneCallStateEvent event = this->phone_call_state_event_queue.front();
      this->phone_call_state_event_queue.pop_front();

      lock.unlock();
      this->handleEventSync(event);
      lock.lock();
   }
}


void doorctrl::UiControler::handleQueuedVideoStreamStateEvents() {

   std::unique_lock<std::mutex> lock(this->queue_mutex);

   while(this->video_stream_state_event_queue.size() > 0) {
      doorctrl::VideoStreamStateEvent event = this->video_stream_state_event_queue.front();
      this->video_stream_state_event_queue.pop_front();
    
      lock.unlock();
      this->handleEventSync(event);
      lock.lock();
   }
}


void doorctrl::UiControler::handleQueuedHomeButtonStateEvents() {

   std::unique_lock<std::mutex> lock(this->queue_mutex);

   while(this->home_button_state_event_queue.size() > 0) {
      doorctrl::HomeButtonStateEvent event = this->home_button_state_event_queue.front();
      this->home_button_state_event_queue.pop_front();

      lock.unlock();
      this->handleEventSync(event);
      lock.lock();
   }
}


void doorctrl::UiControler::enterRingingState() {
  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: incoming call - enter door control panel ringing state");

  this->state = PHONE_CALL_STATE_RINGING;

  this->last_active_window_title = doorctrl::XWindowControler::getActiveWindowTitle();

  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: enter ringing state - current active window title = " + this->last_active_window_title);

  doorctrl::XWindowControler::wakeupScreen();
  doorctrl::XWindowControler::disableScreensaver();
  doorctrl::XWindowControler::bringToFront(this->config_data->getApplicationConfig().app_name);

  /*
   * Starting video stream view may take some while especially when the
   * camera is not available and the connector waits for communication timeout error.  
   * Initialize video view first to avoid that someone answers the incomming call
   * while handling this event. That would cause an phone lock and a phone timeout error. 
   */
  if(this->video_panel != nullptr && this->config_data != nullptr &&
     this->config_data->getVideoConfig().enabled) {

     this->video_panel->enterStreamingState(); 
  }

  if(this->door_control_panel != nullptr) {
     this->door_control_panel->enterRingingState(); 
  }
}


void doorctrl::UiControler::enterEstablishedState() {
  this->state = PHONE_CALL_STATE_ESTABLISHED;
  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: call established - updating door control panel");
  if(this->door_control_panel != nullptr) {
     this->door_control_panel->enterAnsweringState(); 
  }
}


void doorctrl::UiControler::handleHangupButtonEvent(GtkButton* button, gpointer user_data) {
  if(user_data != nullptr) {
     static_cast<doorctrl::UiControler*>(user_data)->doHangup();
  }
}


void doorctrl::UiControler::handleAnswerButtonEvent(GtkButton* button, gpointer user_data) {
  if(user_data != nullptr) {
     static_cast<doorctrl::UiControler*>(user_data)->doAnswer();
  }
}


void doorctrl::UiControler::handleDoorOpenerButtonEvent(GtkButton* button, gpointer user_data) {
  if(user_data != nullptr) {
     static_cast<doorctrl::UiControler*>(user_data)->doDoorOpener();
  }
}


void doorctrl::UiControler::handleSpeakerVolumeChangedEvent(GtkScaleButton* button, double value, gpointer user_data) {
  if(user_data != nullptr) {
     doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: speaker volume has changed, value = " + std::to_string(value));
     static_cast<doorctrl::UiControler*>(user_data)->setSpeakerVolume((uint8_t)value);
  }
}
