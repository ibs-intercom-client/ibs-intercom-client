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

#include "x_window_controler.h"

#include <gtk/gtk.h>
#include <memory>
#include <glib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/dpms.h>
#include <X11/extensions/scrnsaver.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include "gtk_ui_logger.h"
#include <iostream>
#include <atomic>

static std::atomic<bool> screensaver_suspended(false); 


/*
 * Credits
 *
 * Special thanks to the wmctrl project (see https://github.com/dancor/wmctrl/blob/master/main.c)
 * for borrowing the code to use the XLib interface. 
 */ 

class Deleter {
   public:
      void operator()(gchar* buffer) {
         g_free((gpointer*)buffer);
      }
};


#define MAX_PROPERTY_VALUE_LEN 4096

static std::unique_ptr<gchar, Deleter> get_window_title(Display *disp, Window win);
static std::unique_ptr<gchar, Deleter> get_property(Display *disp, Window win, Atom xa_prop_type, gchar *prop_name, unsigned long *size);
static int client_msg(Display *disp, Window win, char *msg, unsigned long data0, unsigned long data1, unsigned long data2, unsigned long data3, unsigned long data4);
static gboolean handle_screensaver_suspend_state(gpointer user_data);


gboolean handle_screensaver_suspend_state(gpointer user_data) {

   if(screensaver_suspended) {
       
      Display *disp = XOpenDisplay(NULL);

      if(disp == nullptr) {
         return true;
      }

      XResetScreenSaver(disp); 

      XCloseDisplay(disp);
   }
   return true;
}


void doorctrl::XWindowControler::setup() {
   gdk_threads_add_timeout(10000, &handle_screensaver_suspend_state, NULL);
}


void doorctrl::XWindowControler::wakeupScreen() {

   doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: wakeup screen");

   Display *disp = XOpenDisplay(NULL);

   if(disp == nullptr) {
      doorctrl::GtkUiLogger::getInstance().getOutput().printError("Error: cannot wakeup screen, cannot open display");
      return;
   }

   DPMSEnable(disp);
   DPMSForceLevel(disp, DPMSModeOn);

   unsigned int keycode = XKeysymToKeycode(disp, XK_F2);
   XTestFakeKeyEvent(disp, keycode, True, 0); 
   XTestFakeKeyEvent(disp, keycode, False, 0); 
   XFlush(disp);

   XCloseDisplay(disp);
}




void doorctrl::XWindowControler::bringToFront(const std::string& requested_window_title) {

   Display *disp = XOpenDisplay(NULL);
   unsigned long client_list_bytes = 0;

   if(disp == nullptr) {
      doorctrl::GtkUiLogger::getInstance().getOutput().printError("Error: cannot bring doorctrl window to front, cannot open display");
      return;
   }

   std::unique_ptr<gchar, Deleter> client_list = get_property(disp, DefaultRootWindow(disp), XA_WINDOW, (gchar*)"_NET_CLIENT_LIST", &client_list_bytes); 

   if(client_list == nullptr) {
      client_list = get_property(disp, DefaultRootWindow(disp), XA_WINDOW, (gchar*)"_WIN_CLIENT_LIST", &client_list_bytes); 

   
      if(client_list == nullptr) {
         doorctrl::GtkUiLogger::getInstance().getOutput().printError("Error: cannot bring doorctrl window to front, empty window list");
        return;
      }
   }

   for(unsigned long i = 0; i < client_list_bytes / sizeof(Window); i++) {
      std::unique_ptr<gchar, Deleter> window_title = get_window_title(disp, ((Window*)client_list.get())[i]);

      if(window_title == nullptr) {
         doorctrl::GtkUiLogger::getInstance().getOutput().printError("Error: cannot bring doorctrl window to front, window title is NULL");
        return;
      }

      doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: window title " + std::to_string(i) + " = " + window_title.get());

      if(strstr(window_title.get(), requested_window_title.c_str()) != nullptr) {
         doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: bringing window " + std::string(window_title.get()) + " to front");

         client_msg(disp, ((Window*)client_list.get())[i], (gchar*)"_NET_ACTIVE_WINDOW", 0, 0, 0, 0, 0);
         XMapRaised(disp, ((Window*)client_list.get())[i]);
      }  
   }

   XCloseDisplay(disp);
}


std::string doorctrl::XWindowControler::getActiveWindowTitle() {

   Display *disp = XOpenDisplay(NULL);
   unsigned long result_bytes = 0;

   if(disp == nullptr) {
      doorctrl::GtkUiLogger::getInstance().getOutput().printError("Error: cannot get active window title, cannot open display");
      return "";
   }

   std::unique_ptr<gchar, Deleter> buffer = get_property(disp, DefaultRootWindow(disp), XA_WINDOW, (gchar*)"_NET_ACTIVE_WINDOW", &result_bytes); 

   if(buffer == nullptr) {
      doorctrl::GtkUiLogger::getInstance().getOutput().printError("Error: cannot get active window title, empty result buffer");
      return "";
   }

   Window active_window = ((Window*)buffer.get())[0];

   std::unique_ptr<gchar, Deleter> window_title = get_window_title(disp, active_window);

   if(window_title == nullptr) {
      doorctrl::GtkUiLogger::getInstance().getOutput().printError("Error: cannot get active window title, window title is NULL");
      return "";
   }

   doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: active window title = " + std::string(window_title.get()));

   XCloseDisplay(disp);

   return std::string(window_title.get());
}


void doorctrl::XWindowControler::disableScreensaver() {

   if(!screensaver_suspended) {
      screensaver_suspended = true; 

      Display *disp = XOpenDisplay(NULL);

      if(disp == nullptr) {
         doorctrl::GtkUiLogger::getInstance().getOutput().printError("Error: cannot disable screen saver, display not set");
         return;
      }

      int event_base = 0;
      int error_base = 0;

      if(!XScreenSaverQueryExtension(disp, &event_base, &error_base)) {
         doorctrl::GtkUiLogger::getInstance().getOutput().printError("Error: cannot disable screen saver, screensaver extension not supported");
         return;
      }

      XScreenSaverSuspend(disp, true); 

      XCloseDisplay(disp);
   }
}


void doorctrl::XWindowControler::enableScreensaver() {

   if(screensaver_suspended) {
      screensaver_suspended = false; 

      Display *disp = XOpenDisplay(NULL);

      if(disp == nullptr) {
         doorctrl::GtkUiLogger::getInstance().getOutput().printError("Error: cannot enable screen saver, display not set");
         return;
      }

      int event_base = 0;
      int error_base = 0;

      if(!XScreenSaverQueryExtension(disp, &event_base, &error_base)) {
         doorctrl::GtkUiLogger::getInstance().getOutput().printError("Error: cannot enable screen saver, screensaver extension not supported");
         return;
      }

      XScreenSaverSuspend(disp, false); 

      XCloseDisplay(disp);
   }
}


std::unique_ptr<gchar, Deleter> get_window_title(Display *disp, Window win) {
    std::unique_ptr<gchar, Deleter> title_utf8;
    std::unique_ptr<gchar, Deleter> wm_name;
    std::unique_ptr<gchar, Deleter> net_wm_name;

    wm_name = get_property(disp, win, XA_STRING, (gchar*)"WM_NAME", NULL);
    net_wm_name = get_property(disp, win, 
            XInternAtom(disp, "UTF8_STRING", False), (gchar*)"_NET_WM_NAME", NULL);

    if (net_wm_name != nullptr) {
        title_utf8 = std::unique_ptr<gchar, Deleter>(g_strdup(net_wm_name.get()));
    }
    else {
        if (wm_name != nullptr) {
            title_utf8 = std::unique_ptr<gchar, Deleter>(g_locale_to_utf8(wm_name.get(), -1, NULL, NULL, NULL));
        }
    }

    return title_utf8;
}



std::unique_ptr<gchar, Deleter> get_property(Display *disp, Window win,
                                             Atom xa_prop_type, gchar *prop_name, 
                                             unsigned long *size) {
    Atom xa_prop_name;
    Atom xa_ret_type;
    int ret_format;
    unsigned long ret_nitems;
    unsigned long ret_bytes_after;
    unsigned long tmp_size;
    unsigned char *ret_prop;
    std::unique_ptr<gchar, Deleter> ret;
    
    xa_prop_name = XInternAtom(disp, prop_name, False);
    
    /* MAX_PROPERTY_VALUE_LEN / 4 explanation (XGetWindowProperty manpage):
     *
     * long_length = Specifies the length in 32-bit multiples of the
     *               data to be retrieved.
     */
    if(XGetWindowProperty(disp, win, xa_prop_name, 0, 
                          MAX_PROPERTY_VALUE_LEN / 4, False,
                          xa_prop_type, &xa_ret_type, &ret_format,     
                          &ret_nitems, &ret_bytes_after, &ret_prop) != Success) {
        doorctrl::GtkUiLogger::getInstance().getOutput().printError("Error: cannot get " + std::string(prop_name) + " property");
        return nullptr;
    }
  
    if (xa_ret_type != xa_prop_type) {
        doorctrl::GtkUiLogger::getInstance().getOutput().printError("Error: invalid type " + std::to_string(xa_ret_type) + " of " + std::string(prop_name) + " property, expecting type " + std::to_string(xa_prop_type));
        XFree(ret_prop);
        return nullptr;
    }

    /* null terminate the result to make string handling easier */
    tmp_size = (ret_format / (32 / sizeof(long))) * ret_nitems;
    ret = std::unique_ptr<gchar, Deleter>((gchar*)g_malloc(tmp_size + 1));
    memcpy(ret.get(), ret_prop, tmp_size);
    (ret.get())[tmp_size] = '\0';

    if (size) {
        *size = tmp_size;
    }
    
    XFree(ret_prop);
    return ret;
}


int client_msg(Display *disp, Window win, char *msg,
               unsigned long data0, unsigned long data1, 
               unsigned long data2, unsigned long data3,
               unsigned long data4) {
  XEvent event;
  long mask = SubstructureRedirectMask | SubstructureNotifyMask;

  event.xclient.type = ClientMessage;
  event.xclient.serial = 0;
  event.xclient.send_event = True;
  event.xclient.message_type = XInternAtom(disp, msg, False);
  event.xclient.window = win;
  event.xclient.format = 32;
  event.xclient.data.l[0] = data0;
  event.xclient.data.l[1] = data1;
  event.xclient.data.l[2] = data2;
  event.xclient.data.l[3] = data3;
  event.xclient.data.l[4] = data4;

  if (XSendEvent(disp, DefaultRootWindow(disp), False, mask, &event)) {
    return EXIT_SUCCESS;
  }
  else {
//    fprintf(stderr, "Cannot send %s event.\n", msg);
    return EXIT_FAILURE;
  }
}
