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

#include "video_panel.h"
#include <mutex>
#include <condition_variable>
#include <gst/gst.h>
#include <gdk/gdkwayland.h>
#include <gdk/gdkx.h>
#include <gst/video/videooverlay.h>
#include "gtk_ui_logger.h"
#include <chrono>
#include <thread> 
#include "common/simple_observer_manager.cpp"

static std::mutex mutex;
static std::mutex wait_mutex;
static std::condition_variable cond;

static void realize_callback(GtkWindow *window, gpointer data);
static gboolean draw_image_callback(GtkWidget *widget, cairo_t* cr, gpointer data);
static void on_pad_added(GstElement* element, GstPad* pad, gpointer data);
static void wait_for_widgets_realized(doorctrl::VideoPanel* panel);

void on_pad_added(GstElement* element, GstPad* pad, gpointer data) {

  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: gstreamer pad " + std::string(gst_pad_get_name(pad)) + " has been added"); 

  GstPad* sinkpad;

  GstElement* queue = (GstElement*)data;

  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: query gstreamer sinkpad"); 

  sinkpad = gst_element_get_static_pad(queue, "sink");

  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: link to gstreamer sinkpad"); 

  gst_pad_link(pad, sinkpad);
  gst_object_unref(sinkpad);
  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: function on_pad_added callback finished"); 
}


doorctrl::VideoPanel::VideoPanel(std::shared_ptr<ConfigData>& config_data) 
  : config_data(config_data)   {

}


void doorctrl::VideoPanel::cleanupPipeline() {

   doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: cleanup video pipeline");

   if(this->pipeline != nullptr) {

      gst_element_send_event (pipeline, gst_event_new_eos ());
      // wait for EOS message on the pipeline bus
      GstMessage* message = gst_bus_timed_pop_filtered(this->gstreamer_bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_EOS | GST_MESSAGE_ERROR));

      if(message != nullptr) { 
         gst_message_unref(message);
      }

      if(this->gstreamer_bus != nullptr) {
         gst_object_unref(this->gstreamer_bus);
      }

      if(this->pipeline != nullptr) {
         gst_element_set_state (this->pipeline, GST_STATE_NULL);
         gst_object_unref(this->pipeline);
      }

      this->pipeline = nullptr;
      this->gstreamer_bus = nullptr;

      this->video_sink = nullptr;
      this->decoder = nullptr;
      this->video_source = nullptr;
      this->demux = nullptr;
      this->queue = nullptr;
      this->video_source = nullptr;
   }
}


void doorctrl::VideoPanel::setupPipeline() {

   doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: setup video pipeline");

    this->video_source = gst_element_factory_make ("souphttpsrc", NULL); g_assert(this->video_source);
   g_object_set (G_OBJECT (this->video_source), "location", this->config_data->getVideoConfig().video_url.c_str(), NULL);


   this->demux = gst_element_factory_make ("multipartdemux", NULL); g_assert(this->demux);
   this->queue = gst_element_factory_make ("queue", NULL); g_assert(this->queue);
   this->decoder = gst_element_factory_make ("jpegdec", NULL); g_assert(this->decoder);

   GdkWindow* window_handle = gtk_widget_get_window (this->image_widget);
   if(GDK_IS_X11_WINDOW(window_handle)) {
     this->video_sink = gst_element_factory_make ("xvimagesink", NULL); g_assert(this->video_sink);
   } else if(GDK_IS_WAYLAND_WINDOW(window_handle)) {
     this->video_sink = gst_element_factory_make ("waylandsink", NULL); g_assert(this->video_sink);
   }

   this->pipeline = gst_pipeline_new ("doorctrl-pipeline");
   this->gstreamer_bus = gst_element_get_bus (this->pipeline);

   //ADD
   gst_bin_add_many (GST_BIN (this->pipeline), this->video_source, this->demux, this->queue, this->decoder, this->video_sink, NULL);

   //LINK
   gst_element_link (this->video_source, this->demux);
   gst_element_link_many ( this->queue, this->decoder, this->video_sink, NULL);
   g_signal_connect(demux, "pad-added", G_CALLBACK(on_pad_added), this->queue);
}

void doorctrl::VideoPanel::setup() {

   this->playing = false;
   this->pipeline = nullptr;

   gst_init(this->config_data->getApplicationConfig().argc_ptr, this->config_data->getApplicationConfig().argv_ptr);

   doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: preparing widgets ...");

   this->layout_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
   this->stack = gtk_stack_new();
   gtk_box_pack_start(GTK_BOX(this->layout_box), this->stack, TRUE, TRUE, 0);

   this->video_widget =  gtk_drawing_area_new ();

   GError* error = NULL;
   this->pixbuf = gdk_pixbuf_new_from_file("/usr/share/doorctrl/images/tranquility.jpg", &error);

   if(error != NULL) {
      doorctrl::GtkUiLogger::getInstance().getOutput().printWarning("Warning: error while loading wall paper, message = " + std::string(error->message));
   }
   this->image_widget = gtk_image_new();
   
   GdkPixbuf* temp_buf = gdk_pixbuf_scale_simple(this->pixbuf, 640, 480, GDK_INTERP_BILINEAR);
   gtk_image_set_from_pixbuf(GTK_IMAGE(this->image_widget), temp_buf);
   g_object_unref(temp_buf);

//   gtk_window_set_default_size(GTK_WINDOW(this->video_widget), 360, 250);
//   gtk_widget_set_size_request(GTK_WIDGET(this->image_widget), 640, 480);

   gtk_widget_show(this->video_widget);
   gtk_widget_show(this->stack);
   gtk_widget_show(this->image_widget);

   gtk_stack_add_named(GTK_STACK(this->stack), this->image_widget, "tranquility");
   gtk_stack_add_named(GTK_STACK(this->stack), this->video_widget, "video");
   doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: switch video panel to idle view");
   gtk_stack_set_visible_child_name(GTK_STACK(this->stack), "tranquility");

   g_signal_connect(G_OBJECT(this->video_widget), "realize", G_CALLBACK(realize_callback), this); 
   g_signal_connect(G_OBJECT(this->image_widget), "draw", G_CALLBACK(draw_image_callback), this); 
}

/*
 * A xv video overlay is used to display the video stream received from the door camera.
 * The size and position of the view port is connected to to GTK widget this way so that
 * the video image is embedded into the application's main window.
 * A X window handle is needed to setup the xv image overlay.
 */ 
void doorctrl::VideoPanel::setupVideoOverlay() {

   doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: entering setup video overlay");
   GdkWindow* video_window_handle = gtk_widget_get_window (this->video_widget);

   if(GDK_IS_X11_WINDOW(video_window_handle)) {

      doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: get X window handle ...");
      gulong embed_xid = GDK_WINDOW_XID (video_window_handle);
      gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY(this->video_sink), (guintptr)embed_xid);
   } else if(GDK_IS_WAYLAND_WINDOW(video_window_handle)) {

      doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: get Wayland window handle ...");

      GtkAllocation allocation = {0, 0, 0, 0};
      wl_surface* wayland_window_handle = gdk_wayland_window_get_wl_surface(video_window_handle);
      gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(this->video_sink), (guintptr)wayland_window_handle);
      gst_video_overlay_set_render_rectangle(GST_VIDEO_OVERLAY(this->video_sink), allocation.x, allocation.y, allocation.width, allocation.height);
   }


   /*
    * The video widget is realized when the stack widget shows the video stream
    * When the video widget is not connected to the xvimage overlay the
    * pipeline won't start properly. To avoid this race condition the
    * pipeline is just started again when the video stream should be played. 
    */  
   std::lock_guard<std::mutex> guard(mutex); 
   if(this->playing) {
      if (gst_element_get_state (this->pipeline, NULL, NULL, -1) == GST_STATE_CHANGE_FAILURE) {
         doorctrl::GtkUiLogger::getInstance().getOutput().printWarning("Warning: error while running video stream, can not enter PLAYING state");

         this->cleanupPipeline();

         doorctrl::VideoStreamStateEvent event(doorctrl::VIDEO_STREAM_STATE_ERROR);
         this->video_observer_manager.fireEvent(event);
         gtk_stack_set_visible_child_name(GTK_STACK(this->stack), "tranquility");
      } else {
         doorctrl::VideoStreamStateEvent event(doorctrl::VIDEO_STREAM_STATE_RUNNING);
         this->video_observer_manager.fireEvent(event);
         doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: video pipeline state = " + gst_element_get_state (this->pipeline, NULL, NULL, -1));
      }
   } else {
      gst_element_set_state (this->pipeline, GST_STATE_NULL);
   }
}



void doorctrl::VideoPanel::enterStreamingState() {

   doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: enter streaming state");

   {
      std::lock_guard<std::mutex> guard(mutex); 
      this->playing = true;
   }

   /*
    * The video pipeline starts when the xvimage overlay has been
    * connected to its widget. Because the widget is realized asynchronously
    * this may be a race condition. There is no way to control that the
    * widget has a XWindow handle before the pipeline starts.
    * Thus it is essential to bring the video panel on top of the video widget. 
    * The video panel is realized then ang gets a X window handle which is 
    * required to create the video overlay correctly. If the X window handle is
    * not ready the video view will be opened in a separate window which is not
    * connected to the doorctrl main window. 
    * There have been a lot o testing and investigation to find out the correct order
    * of stream and window creation. Without bringing the video on top
    * first everything was really bad. No chance to do it right when the
    * video panel is not open.
    */ 
   doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: realizing gtk widgets");
   this->setupPipeline();
//   gst_element_set_state (this->pipeline, GST_STATE_PLAYING);
   gtk_stack_set_visible_child_name(GTK_STACK(this->stack), "video");

   /*
    * This call triggers widget creation process to get the video panel's X window handle.
    * This is done asynchronously. This call will return immediately and the widgets are created
    * later by the main event loop. 
    * Thus, just call setupVideoOverlay when the widgets and X window handles are already realized 
    * so that gtk_widget_get_realized() returns true 
    * The X window handle MUST be present befor the streaming pipeline and the video image overlay
    * is initialized. Otherwise a separate X window will be opened which is not connected with the
    * applications main window.
    */
   if(gtk_widget_get_realized(this->video_widget)) {

      gst_element_set_state (this->pipeline, GST_STATE_PLAYING);
      doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: widgets already realized, realized flag = " + std::to_string(this->isVideoPanelRealized()) + ", setting up video overlay");
      setupVideoOverlay();

   } else {

      /*
       * This is the result of many testing cycles. Switching from video into idle view
       * fails when doing this in the realize-callback function. Separating overlay initialisation
       * and panel switching in an separate thread attemped to solve this problem.
       * Finally opening the video panel before initializing the video overlay is essential. 
       * This mechanism may be kept as emergency fallback.
       */
      doorctrl::GtkUiLogger::getInstance().getOutput().printError("Error: starting wait thread");
      this->realize_wait_thread = std::make_unique<std::thread>(wait_for_widgets_realized, this);
   }

   gst_element_set_state (this->pipeline, GST_STATE_PLAYING);
}


void doorctrl::VideoPanel::enterIdleState() {

   this->playing = false;

   /*
    * stopping pipeline first causes the view to freeze without returning
    * to idle view
    * Pipeline state MUST be set to zero to stop video stream. Otherwise
    * the video stream is written to a timeshift storage an then
    * played in hyperspeed next time. 
    * Deleting the whole pipeline does not help.
    */

   if(this->pipeline != nullptr) {
      this->cleanupPipeline();

      doorctrl::VideoStreamStateEvent event(doorctrl::VIDEO_STREAM_STATE_STOPPED);
      this->video_observer_manager.fireEvent(event);

      this->video_sink = nullptr;
      this->decoder = nullptr;
      this->video_source = nullptr;
      this->demux = nullptr;
      this->queue = nullptr;
   }

   /*
    * do not close or hide the video widget before video shutdown has been
    * finished
    * No sleep required here when doing it right!!! UI modification MUST NOT
    * be done in a thread other than the main thread.
    */
   doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: switch video panel to idle view");
   gtk_stack_set_visible_child_name(GTK_STACK(this->stack), "tranquility");
}


/*
 * This is the video panel's main widget which is used as GTK handle
 */
GtkWidget* doorctrl::VideoPanel::getHandle() const {

   return this->layout_box;
}


/*
 * Adapt image size to widget size. This is not done automatically.
 */
void doorctrl::VideoPanel::resizeImage(int width, int height) {

   doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: resize image, width = " + std::to_string(width) + ", height = " + std::to_string(height));

   /*
    * check if image size has really changed because this method is always
    * called by a widget draw operation.
    * When the size is the me the image must not be resized otherwise
    * a nice infinite loop starts here because setting the image always
    * triggers another call to widget draw()
    */ 
   if(width != this->current_width || height != this->current_height) {
      if(this->pixbuf != nullptr) {
         GdkPixbuf* temp_buf = gdk_pixbuf_scale_simple(this->pixbuf, width, height, GDK_INTERP_BILINEAR);
         gtk_image_set_from_pixbuf(GTK_IMAGE(this->image_widget), temp_buf);
         g_object_unref(temp_buf);
      }
      this->current_width = width;
      this->current_height = height;
   }
}


void doorctrl::VideoPanel::setVideoPanelRealized(bool realized) {
  std::lock_guard<std::mutex> guard(mutex); 
  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: set video panel realized = " + std::to_string(realized)); 
  this->video_panel_realized = realized;
}

bool doorctrl::VideoPanel::isVideoPanelRealized() const {
  std::lock_guard<std::mutex> guard(mutex); 
  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: is video panel realized = " + std::to_string(this->video_panel_realized)); 
  return this->video_panel_realized;
}


void realize_callback(GtkWindow *window, gpointer data) {
   doorctrl::VideoPanel* panel = static_cast<doorctrl::VideoPanel*>(data);

   {
      std::lock_guard<std::mutex> guard(wait_mutex); 
      panel->setVideoPanelRealized(true);
   }

   doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: realize callback");
   cond.notify_all();
   doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: realize callback finished");
}


gboolean draw_image_callback(GtkWidget *widget, cairo_t* cr, gpointer data) {
   doorctrl::VideoPanel* panel = static_cast<doorctrl::VideoPanel*>(data);
   GtkAllocation allocation;
   gtk_widget_get_allocation(widget, &allocation);

   panel->resizeImage(allocation.width, allocation.height);

   return FALSE;
}


void wait_for_widgets_realized(doorctrl::VideoPanel* panel) {
  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: waiting for widgets realized");

  /*
   * lock any changes to realize state till wait condition is set
   * when the condition arrives before setting the condition the wait will
   * pause thread execution forever and the video overlay is never created 
   */
  std::unique_lock<std::mutex> lock(wait_mutex);
  if(!panel->isVideoPanelRealized()) {
     doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: video widget not realized, entering wait condition");
     /*
      * this will unlock the mutex when the wait condition is armed
      */
     cond.wait(lock);
  }

  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: wait condition returned, start video stream");

  panel->setupVideoOverlay();
}


void doorctrl::VideoPanel::addObserver(const std::shared_ptr<doorctrl::VideoStreamStateObserver>& observer) {
  return this->video_observer_manager.addObserver(observer);
}


void doorctrl::VideoPanel::removeObserver(const std::shared_ptr<doorctrl::VideoStreamStateObserver>& observer) {
  this->video_observer_manager.removeObserver(observer);
}

