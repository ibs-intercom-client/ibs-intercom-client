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

#include "gstreamer_ringtone_player.h"
#include "gstreamer_media_logger.h"
#include "common/ringtone_player_setup_exception.h"
#include <thread>
#include <homegear-base/HelperFunctions/Io.h>


doorctrl::GStreamerRingtonePlayer::GStreamerRingtonePlayer(std::shared_ptr<ConfigData>& config_data)
  : config_data(config_data) {

  this->volume = config_data->getRingtoneConfig().volume; 
  this->mute = config_data->getRingtoneConfig().mute; 
}


doorctrl::GStreamerRingtonePlayer::~GStreamerRingtonePlayer() {
  this->config_data->removeObserver(shared_from_this());
}


void doorctrl::GStreamerRingtonePlayer::setup() {
   this->config_data->addObserver(shared_from_this());

   if(!BaseLib::Io::fileExists(this->config_data->getRingtoneConfig().path)){
      doorctrl::GStreamerMediaLogger::getInstance().getOutput().printError("Error: cannot find configured ringtone file '" + this->config_data->getRingtoneConfig().path + "'");
      throw doorctrl::RingtonePlayerSetupException("Cannot find configured ringtone file '" + this->config_data->getRingtoneConfig().path + "'");
   }
}


void doorctrl::GStreamerRingtonePlayer::setupPipeline(bool loop) {
   GstElement* filesrc = nullptr;
   GstElement* mpegaudioparse = nullptr;
   GstElement* mpg123audiodec = nullptr;
   GstElement* audioconvert = nullptr;
   GstElement* audioresample = nullptr;
   GstElement* sink = nullptr;

   doorctrl::GStreamerMediaLogger::getInstance().getOutput().printDebug("Debug: setup pipeline");
   this->pipeline = gst_pipeline_new("mp3player");
   this->gstreamer_bus = gst_element_get_bus (this->pipeline);

   filesrc = gst_element_factory_make ("multifilesrc", NULL);
   mpegaudioparse = gst_element_factory_make ("mpegaudioparse", NULL);
   mpg123audiodec = gst_element_factory_make ("mpg123audiodec", NULL);
   audioconvert = gst_element_factory_make ("audioconvert", NULL);
   audioresample = gst_element_factory_make ("audioresample", NULL);
   this->volume_element = gst_element_factory_make ("volume", NULL);
   sink = gst_element_factory_make ("alsasink", NULL);

   g_assert(filesrc);
   g_assert(mpegaudioparse);
   g_assert(mpg123audiodec);
   g_assert(audioconvert);
   g_assert(audioresample);
   g_assert(this->volume_element);
   g_assert(sink);

   if(!BaseLib::Io::fileExists(this->config_data->getRingtoneConfig().path)){
      doorctrl::GStreamerMediaLogger::getInstance().getOutput().printError("Error: cannot find configured ringtone file '" + this->config_data->getRingtoneConfig().path + "'");
   } else {
      doorctrl::GStreamerMediaLogger::getInstance().getOutput().printDebug("Debug: setup ringtone file '" + this->config_data->getRingtoneConfig().path + "'");
   }

   /* Set the URI to play */
   g_object_set (filesrc, "location", this->config_data->getRingtoneConfig().path.c_str(), NULL);

   doorctrl::GStreamerMediaLogger::getInstance().getOutput().printDebug("Debug: setuping up loop property, loop = " + std::to_string(loop));

   if(loop) {
      g_object_set (filesrc, "loop", TRUE, NULL);
   } else {
      g_object_set (filesrc, "loop", FALSE, NULL);
      g_object_set (filesrc, "stop-index", 0, NULL);
   }

   g_object_set (this->volume_element, "volume", ((double)(this->volume) / 100.0), NULL);
   g_object_set (this->volume_element, "mute", this->mute, NULL);


   //ADD
   gst_bin_add_many (GST_BIN (this->pipeline), filesrc, mpegaudioparse, mpg123audiodec , audioconvert, audioresample, this->volume_element, sink, NULL);

   //LINK
   g_assert(gst_element_link (filesrc, mpegaudioparse));
   g_assert(gst_element_link (mpegaudioparse, mpg123audiodec));
   g_assert(gst_element_link (mpg123audiodec, audioconvert));
   g_assert(gst_element_link (audioconvert, audioresample));
   g_assert(gst_element_link (audioresample, this->volume_element));
   g_assert(gst_element_link (this->volume_element, sink));
}


void doorctrl::GStreamerRingtonePlayer::cleanupPipeline() {
   if(this->pipeline != nullptr) {

      gst_object_unref(this->gstreamer_bus);
      gst_element_set_state (this->pipeline, GST_STATE_NULL);
      gst_object_unref(this->pipeline);

      this->pipeline = nullptr;
      this->gstreamer_bus = nullptr;
   }
}


void doorctrl::GStreamerRingtonePlayer::cleanup() {
  this->stopRinging();
}


void doorctrl::GStreamerRingtonePlayer::startRinging() {

   std::lock_guard<std::mutex> lock(this->player_mutex);

   doorctrl::GStreamerMediaLogger::getInstance().getOutput().printDebug("Debug: start ringing");

   if (this->pipeline != nullptr) {
     
     GstState current_state;
     GstState pending_state;
     gst_element_get_state (this->pipeline, &current_state, &pending_state, -1);

     if(current_state == GST_STATE_PLAYING) {
        doorctrl::GStreamerMediaLogger::getInstance().getOutput().printInfo("Info: cannot start ringing, ringtone player already running");
        return;
     } else {
        this->cleanupPipeline();
     }
   }

   this->setupPipeline();
   gst_element_set_state (this->pipeline, GST_STATE_PLAYING);

   if (gst_element_get_state (this->pipeline, NULL, NULL, -1) == GST_STATE_CHANGE_FAILURE) {
     doorctrl::GStreamerMediaLogger::getInstance().getOutput().printError("Error: error while running ringtone player, can not enter PLAYING state");
   }
}

void doorctrl::GStreamerRingtonePlayer::ringOnce() {

   std::lock_guard<std::mutex> lock(this->player_mutex);

   if(this->pipeline != nullptr) {
     
     GstState current_state;
     GstState pending_state;
     gst_element_get_state (this->pipeline, &current_state, &pending_state, -1);

     if(current_state == GST_STATE_PLAYING) {
        doorctrl::GStreamerMediaLogger::getInstance().getOutput().printInfo("Info: cannot ring once, ringtone player already running");
        return;
     }
   }

   if(this->ring_once_thread != nullptr) {
      this->ring_once_thread->join();
   }

   this->ring_once_thread =
     std::make_unique<std::thread>(std::bind(&doorctrl::GStreamerRingtonePlayer::handleRingOnce, this));
}


void doorctrl::GStreamerRingtonePlayer::handleRingOnce() {   

   uint8_t duration = this->config_data->getRingtoneConfig().duration; 

   if(duration <= 0 || duration > 10) {
      duration = 2;
   }

   this->startRinging();
   std::this_thread::sleep_for(std::chrono::seconds(duration));
   this->stopRinging();
}


void doorctrl::GStreamerRingtonePlayer::stopRinging() {

   std::lock_guard<std::mutex> lock(this->player_mutex);

   doorctrl::GStreamerMediaLogger::getInstance().getOutput().printDebug("Debug: stop ringing");

   this->cleanupPipeline();
}


void doorctrl::GStreamerRingtonePlayer::handleEvent(const ConfigDataChangedEvent& event) {

   doorctrl::GStreamerMediaLogger::getInstance().getOutput().printDebug("Debug: config data changed, section = " +  event.getChangedDataSection());

   if(event.getChangedDataSection().compare(doorctrl::ConfigDataChangedEvent::RINGTONE_SECTION_NAME) == 0) {  

      doorctrl::GStreamerMediaLogger::getInstance().getOutput().printDebug("Debug: updating ringtone player");

      this->volume = config_data->getRingtoneConfig().volume; 
      this->mute = config_data->getRingtoneConfig().mute; 
   
      std::lock_guard<std::mutex> lock(this->player_mutex);

      if (this->pipeline != nullptr && this->volume_element != nullptr) {
         g_object_set (this->volume_element, "volume", ((double)(this->volume) / 100.0), NULL);
         g_object_set (this->volume_element, "mute", this->mute, NULL);
      }
   }
}


void doorctrl::GStreamerRingtonePlayer::setVolume(uint8_t volume) {

  this->volume = volume;

  std::lock_guard<std::mutex> lock(this->player_mutex);
  if (this->pipeline != nullptr && this->volume_element != nullptr) {
     g_object_set (this->volume_element, "volume", ((double)(this->volume) / 100.0), NULL);
  }
}


uint8_t doorctrl::GStreamerRingtonePlayer::getVolume() {
  return this->volume;
}


void doorctrl::GStreamerRingtonePlayer::setMute(bool mute) {

  this->mute = mute;

  std::lock_guard<std::mutex> lock(this->player_mutex);
  if (this->pipeline != nullptr && this->volume_element != nullptr) {
     g_object_set (this->volume_element, "mute", this->mute, NULL);
  }
}


bool doorctrl::GStreamerRingtonePlayer::getMute() {
   return this->mute;
}

