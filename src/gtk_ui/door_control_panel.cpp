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

#include "door_control_panel.h"
#include "ui_controler.h"
#include "gtk_ui_logger.h"
#include "door_control_button_monkey.h"


void doorctrl::DoorControlPanel::handleMuteButtonEvent(GtkToggleButton* button, gpointer user_data) {
  if(user_data != nullptr) {
     static_cast<doorctrl::DoorControlPanel*>(user_data)->doMuteMic(gtk_toggle_button_get_active(button));
  }
}


void doorctrl::DoorControlPanel::handleVolumeButtonEvent(GtkButton* button, gpointer user_data) {
  if(user_data != nullptr) {
     static_cast<doorctrl::DoorControlPanel*>(user_data)->openSpeakerVolumePopover();
  }
}


void doorctrl::DoorControlPanel::handleVolumeUpButtonEvent(GtkButton* button, gpointer user_data) {
  if(user_data != nullptr) {
     static_cast<doorctrl::DoorControlPanel*>(user_data)->incrementSpeakerVolume();
  }
}


void doorctrl::DoorControlPanel::handleVolumeDownButtonEvent(GtkButton* button, gpointer user_data) {
  if(user_data != nullptr) {
     static_cast<doorctrl::DoorControlPanel*>(user_data)->decrementSpeakerVolume();
  }
}


void doorctrl::DoorControlPanel::openSpeakerVolumePopover() {

  int8_t volume = this->controler->getSpeakerVolume(); 
  gchar *str_value = g_strdup_printf("%i%%", (int)volume);
  gtk_label_set_text(GTK_LABEL(this->volume_value_label), str_value);

  gtk_popover_popup(GTK_POPOVER(this->volume_popover));

}


void doorctrl::DoorControlPanel::incrementSpeakerVolume() {

  int8_t volume = this->controler->getSpeakerVolume(); 
  volume += 10;
  if(volume > 100) {
    volume = 100;
  }
  this->controler->setSpeakerVolume(volume);

  gchar *str_value = g_strdup_printf("%i%%", (int)volume);
  gtk_label_set_text(GTK_LABEL(this->volume_value_label), str_value);
}


void doorctrl::DoorControlPanel::decrementSpeakerVolume() {

  int8_t volume = this->controler->getSpeakerVolume(); 
  volume -= 10;
  if(volume < 0) {
    volume = 0;
  }
  this->controler->setSpeakerVolume(volume);

  gchar *str_value = g_strdup_printf("%i%%", (int)volume);
  gtk_label_set_text(GTK_LABEL(this->volume_value_label), str_value);
}


void doorctrl::DoorControlPanel::setup(std::shared_ptr<UiControler>& controler) {

  this->has_video_stream_error = false;
  this->controler = controler;

  this->monkey = std::make_unique<doorctrl::DoorControlButtonMonkey>();

  this->layout_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6));
  gtk_box_set_spacing(GTK_BOX(this->layout_box), 10);
  gtk_widget_set_name(GTK_WIDGET(this->layout_box), (gchar*)"door_control_panel");
  gtk_widget_set_size_request(GTK_WIDGET(this->layout_box), 250, 100);


  this->setupCenterBox();
  this->setupButtons();

//  this->setupButtonMonkey();

  gtk_container_set_border_width(GTK_CONTAINER(this->layout_box), 10);

  gtk_box_set_center_widget(this->layout_box, GTK_WIDGET(this->center_box));

  gtk_box_pack_end(this->layout_box, GTK_WIDGET(this->phone_button_box), FALSE, FALSE, 6);
}


void doorctrl::DoorControlPanel::setupCenterBox() {

  this->center_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 6));

  this->error_subline = gtk_label_new("Kamera nicht verfügbar");
  gtk_widget_set_name(GTK_WIDGET(this->error_subline), (gchar*)"error_subline");
  gtk_widget_hide(this->error_subline);

  this->setupStateIndicator();

  gtk_box_pack_start(this->center_box, GTK_WIDGET(this->state_indicator), TRUE, TRUE, 2);
  gtk_box_pack_start(this->center_box, GTK_WIDGET(this->error_subline), TRUE, TRUE, 2);

}


void doorctrl::DoorControlPanel::setupStateIndicator() {

  this->ready = gtk_label_new("Bereit");
  gtk_widget_set_name(GTK_WIDGET(this->ready), (gchar*)"ready_label");
  this->ringing = gtk_label_new("Ankommender Ruf");
  gtk_widget_set_name(GTK_WIDGET(this->ringing), (gchar*)"ringing_label");
  this->connecting = gtk_label_new("Verbindungsaufbau...");
  gtk_widget_set_name(GTK_WIDGET(this->connecting), (gchar*)"connecting_label");

  /*
   * no extra status message needed, removing this message will tidy up
   * the screen 
   */
  //this->established = gtk_label_new("Gespräch aktiv");
  this->established = gtk_label_new("");
  gtk_widget_set_name(GTK_WIDGET(this->established), (gchar*)"established_label");
  gtk_widget_show(this->state_indicator);
  gtk_widget_show(this->ready);
  gtk_widget_show(this->ringing);
  gtk_widget_show(this->established);
  gtk_widget_show(this->connecting);

  this->state_indicator = gtk_stack_new();
  gtk_widget_set_name(GTK_WIDGET(this->state_indicator), (gchar*)"state_indicator");

  gtk_stack_add_named(GTK_STACK(this->state_indicator), this->ready, "ready");
  gtk_stack_add_named(GTK_STACK(this->state_indicator), this->ringing, "ringing");
  gtk_stack_add_named(GTK_STACK(this->state_indicator), this->established, "established");
  gtk_stack_add_named(GTK_STACK(this->state_indicator), this->connecting, "connecting");
  gtk_stack_set_visible_child_name(GTK_STACK(this->state_indicator), "ready");

  this->state_label_style_provider = gtk_css_provider_new();

  GtkStyleContext* style_context = gtk_widget_get_style_context(this->ready);
  gtk_style_context_add_provider(style_context, GTK_STYLE_PROVIDER(this->state_label_style_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

  style_context = gtk_widget_get_style_context(this->ringing);
  gtk_style_context_add_provider(style_context, GTK_STYLE_PROVIDER(this->state_label_style_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

  style_context = gtk_widget_get_style_context(this->established);
  gtk_style_context_add_provider(style_context, GTK_STYLE_PROVIDER(this->state_label_style_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

  style_context = gtk_widget_get_style_context(this->connecting);
  gtk_style_context_add_provider(style_context, GTK_STYLE_PROVIDER(this->state_label_style_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

}


void doorctrl::DoorControlPanel::setupButtons() {

  this->phone_button_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6));
  gtk_box_set_spacing(GTK_BOX(this->phone_button_box), 6);
  gtk_widget_set_name(GTK_WIDGET(this->phone_button_box), (gchar*)"phone_button_box");

  this->setupActivityButton();
  this->setupHangupButton();
  this->setupMuteButton();
//  this->setupVolumeButton();
  this->setupMediaBox();

  gtk_box_pack_start(GTK_BOX(this->phone_button_box), GTK_WIDGET(this->mute_button), TRUE, FALSE, 0);
//  gtk_box_pack_start(GTK_BOX(this->phone_button_box), GTK_WIDGET(this->media_box), TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(this->phone_button_box), GTK_WIDGET(this->volume_button), TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(this->phone_button_box), GTK_WIDGET(this->hangup_button), TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(this->phone_button_box), GTK_WIDGET(this->activity_button), TRUE, FALSE, 0);
}


void doorctrl::DoorControlPanel::setupActivityButton() {

  this->activity_button = gtk_button_new_with_label("Annehmen");
  gtk_widget_set_size_request(this->activity_button, 160, 64); 
  gtk_widget_set_name(GTK_WIDGET(this->activity_button), (gchar*)"activity_button");
  this->activity_signal_id = g_signal_connect(G_OBJECT(this->activity_button), "clicked", G_CALLBACK(doorctrl::UiControler::handleAnswerButtonEvent), controler.get()); 

  this->activity_button_style_provider = gtk_css_provider_new();

  GtkStyleContext* style_context = gtk_widget_get_style_context(this->activity_button);
  gtk_style_context_add_provider(style_context, GTK_STYLE_PROVIDER(this->activity_button_style_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}


void doorctrl::DoorControlPanel::setupHangupButton() {

  this->hangup_button = gtk_button_new_with_label("Auflegen");
  gtk_widget_set_size_request(this->hangup_button, 160, 64); 
  gtk_widget_set_name(GTK_WIDGET(this->hangup_button), (gchar*)"hangup_button");
  g_signal_connect(G_OBJECT(this->hangup_button), "clicked", G_CALLBACK(doorctrl::UiControler::handleHangupButtonEvent), controler.get()); 

  this->hangup_button_style_provider = gtk_css_provider_new();

  GtkStyleContext* style_context = gtk_widget_get_style_context(this->hangup_button);
  gtk_style_context_add_provider(style_context, GTK_STYLE_PROVIDER(this->hangup_button_style_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

}


void doorctrl::DoorControlPanel::setupMuteButton() {

  GError* error = NULL;
  GdkPixbuf* mute_pixbuf = gdk_pixbuf_new_from_file("/usr/share/doorctrl/images/mute-microphone-filled.svg", &error);
  this->mute_mic_buf = gdk_pixbuf_scale_simple(mute_pixbuf, 36, 42, GDK_INTERP_BILINEAR);
  this->mute_mic_icon = gtk_image_new_from_pixbuf(this->mute_mic_buf);
  g_object_ref(this->mute_mic_icon);
  g_object_unref(mute_pixbuf);


  GdkPixbuf* mic_pixbuf = gdk_pixbuf_new_from_file("/usr/share/doorctrl/images/microphone-filled.svg", &error);
  this->mic_buf = gdk_pixbuf_scale_simple(mic_pixbuf, 34, 39, GDK_INTERP_BILINEAR);
  this->mic_icon = gtk_image_new_from_pixbuf(this->mic_buf);
  g_object_ref(this->mic_icon);
  g_object_unref(mic_pixbuf);

  this->mute_button = gtk_toggle_button_new();
  gtk_button_set_image(GTK_BUTTON(this->mute_button), this->mic_icon);
  gtk_widget_set_size_request(this->mute_button, 64, 64); 
  gtk_widget_set_name(GTK_WIDGET(this->mute_button), (gchar*)"mute_button");

  g_signal_connect(G_OBJECT(this->mute_button), "toggled", G_CALLBACK(doorctrl::DoorControlPanel::handleMuteButtonEvent), this); 

  this->mute_button_style_provider = gtk_css_provider_new();

  GtkStyleContext* style_context = gtk_widget_get_style_context(this->mute_button);
  gtk_style_context_add_provider(style_context, GTK_STYLE_PROVIDER(this->mute_button_style_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}


void doorctrl::DoorControlPanel::setupVolumeButton() {

  GError* error = NULL;
  GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file("/usr/share/doorctrl/images/speaker-filled.svg", &error);
  GdkPixbuf* temp_buf = gdk_pixbuf_scale_simple(pixbuf, 36, 42, GDK_INTERP_BILINEAR);
  this->volume_icon = gtk_image_new_from_pixbuf(temp_buf);
  g_object_unref(temp_buf);
  g_object_unref(pixbuf);

  this->volume_button = gtk_volume_button_new();
  gtk_button_set_image(GTK_BUTTON(this->volume_button), this->volume_icon);
  gtk_orientable_set_orientation(GTK_ORIENTABLE(this->volume_button), GTK_ORIENTATION_HORIZONTAL); 
  gtk_popover_set_position(GTK_POPOVER(gtk_scale_button_get_popup(GTK_SCALE_BUTTON(this->volume_button))), GTK_POS_LEFT); 

  GtkAdjustment* volume_adjustment = gtk_adjustment_new(50, 0, 100, 10, 10, 10); 
  gtk_scale_button_set_adjustment(GTK_SCALE_BUTTON(this->volume_button), volume_adjustment);
  gtk_widget_set_size_request(this->volume_button, 64, 64); 

/*
  GtkWidget* popover_widget = gtk_scale_button_get_popup(GTK_SCALE_BUTTON(this->volume_button));
  GtkWidget* box_widget = gtk_bin_get_child(GTK_BIN(popover_widget));

  GList* children = gtk_container_get_children(GTK_CONTAINER(box_widget));
  
  if(children != nullptr) {
     for (guint i = 0; i < g_list_length(children); i++) {
        doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: doorctrl panel, searching child widget, current name = " + gtk_widget_get_name(GTK_WIDGET(g_list_nth_data(children, i))));
        if(strncmp(gtk_widget_get_name(GTK_WIDGET(g_list_nth_data(children, i))), "GtkScale", 9) == 0) {
           gtk_container_remove(GTK_CONTAINER(box_widget), GTK_WIDGET(g_list_nth_data(children, i)));
           break;
        }
     }
  } 
*/

  gtk_widget_set_name(GTK_WIDGET(this->volume_button), (gchar*)"volume_button");

  g_signal_connect(G_OBJECT(this->volume_button), "value-changed", G_CALLBACK(doorctrl::UiControler::handleSpeakerVolumeChangedEvent), controler.get()); 

  this->volume_button_style_provider = gtk_css_provider_new();

  GtkStyleContext* style_context = gtk_widget_get_style_context(this->volume_button);
  gtk_style_context_add_provider(style_context, GTK_STYLE_PROVIDER(this->volume_button_style_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}
  

void doorctrl::DoorControlPanel::setupMediaBox() {

  GError* error = NULL;
  GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file("/usr/share/doorctrl/images/speaker-filled.svg", &error);
  GdkPixbuf* temp_buf = gdk_pixbuf_scale_simple(pixbuf, 30, 32, GDK_INTERP_BILINEAR);
  volume_icon = gtk_image_new_from_pixbuf(temp_buf);
  g_object_unref(temp_buf);
  g_object_unref(pixbuf);

  this->volume_button = gtk_button_new();
  gtk_button_set_image(GTK_BUTTON(this->volume_button), this->volume_icon);
  gtk_widget_set_size_request(this->volume_button, 64, 64); 

  gtk_widget_set_name(GTK_WIDGET(this->volume_button), (gchar*)"volume_button");


  this->volume_popover = gtk_popover_new(this->volume_button);
  gtk_widget_set_size_request(GTK_WIDGET(this->volume_popover), 200, 95); 
  gtk_popover_set_position(GTK_POPOVER(this->volume_popover), GTK_POS_LEFT); 


  this->media_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2));
  gtk_widget_set_name(GTK_WIDGET(this->media_box), (gchar*)"media_box");


  this->volume_up_button = gtk_button_new_with_label("+");
  gtk_widget_set_size_request(this->volume_up_button, 85, 90); 
  gtk_widget_set_name(GTK_WIDGET(this->volume_up_button), (gchar*)"volume_up_button");


  this->volume_value_label = gtk_label_new("50%");
  gtk_widget_set_size_request(this->volume_value_label, 200, 90); 
  gtk_label_set_justify(GTK_LABEL(this->volume_value_label), GTK_JUSTIFY_CENTER);
  gtk_widget_set_name(GTK_WIDGET(this->volume_value_label), (gchar*)"volume_value_label");


  this->volume_down_button = gtk_button_new_with_label("-");
  gtk_widget_set_size_request(this->volume_down_button, 85, 90); 
  gtk_widget_set_name(GTK_WIDGET(this->volume_down_button), (gchar*)"volume_down_button");


  gtk_box_pack_start(GTK_BOX(this->media_box), GTK_WIDGET(this->volume_down_button), TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(this->media_box), GTK_WIDGET(this->volume_value_label), TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(this->media_box), GTK_WIDGET(this->volume_up_button), TRUE, FALSE, 0);

  gtk_widget_show(GTK_WIDGET(this->media_box));
  gtk_widget_show(this->volume_down_button);
  gtk_widget_show(this->volume_up_button);
  gtk_widget_show(this->volume_value_label);
  gtk_container_add(GTK_CONTAINER(this->volume_popover), GTK_WIDGET(this->media_box));

  g_signal_connect(G_OBJECT(this->volume_button), "clicked", G_CALLBACK(doorctrl::DoorControlPanel::handleVolumeButtonEvent), this); 
  g_signal_connect(G_OBJECT(this->volume_up_button), "clicked", G_CALLBACK(doorctrl::DoorControlPanel::handleVolumeUpButtonEvent), this); 
  g_signal_connect(G_OBJECT(this->volume_down_button), "clicked", G_CALLBACK(doorctrl::DoorControlPanel::handleVolumeDownButtonEvent), this); 

  this->volume_button_style_provider = gtk_css_provider_new();

  GtkStyleContext* style_context = gtk_widget_get_style_context(this->volume_button);
  gtk_style_context_add_provider(style_context, GTK_STYLE_PROVIDER(this->volume_button_style_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}
  

void doorctrl::DoorControlPanel::setupButtonMonkey() {

  this->monkey = std::make_unique<doorctrl::DoorControlButtonMonkey>();

  this->monkey->setActivityButton(this->activity_button); 
  this->monkey->setHangupButton(this->hangup_button); 

  this->monkey->start();
}


void doorctrl::DoorControlPanel::doMuteMic(bool mute) {

  if(mute) {
     gtk_button_set_image(GTK_BUTTON(this->mute_button), this->mute_mic_icon);
  } else {
     gtk_button_set_image(GTK_BUTTON(this->mute_button), this->mic_icon);
  }

  this->controler->doMuteMic(mute);
}


GtkWidget* doorctrl::DoorControlPanel::getHandle() const {
  return GTK_WIDGET(this->layout_box);
}


void doorctrl::DoorControlPanel::enterRingingState() {

  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: doorctrl panel, enter ringing state");

  gtk_widget_show(this->activity_button);
  gtk_widget_show(this->hangup_button);

  if(this->has_video_stream_error) {
     gtk_widget_show(this->error_subline);
  } else {
     gtk_widget_hide(this->error_subline);
  }

  gtk_stack_set_visible_child_name(GTK_STACK(this->state_indicator), "ringing");

}


void doorctrl::DoorControlPanel::enterIdleState() {

  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: doorctrl panel, enter ready state");

  g_signal_handler_disconnect(G_OBJECT(this->activity_button), this->activity_signal_id); 

  this->changeButtonText(this->activity_button, "Annehmen");

  gtk_stack_set_visible_child_name(GTK_STACK(this->state_indicator), "established");

  gtk_widget_hide(this->mute_button);
  gtk_widget_hide(this->volume_button);
  gtk_widget_hide(this->activity_button);
  gtk_widget_hide(this->hangup_button);
  gtk_widget_hide(this->error_subline);
  gtk_stack_set_visible_child_name(GTK_STACK(this->state_indicator), "ready");

  this->activity_signal_id = g_signal_connect(G_OBJECT(this->activity_button), "clicked", G_CALLBACK(doorctrl::UiControler::handleAnswerButtonEvent), this->controler.get()); 

}


void doorctrl::DoorControlPanel::enterAnsweringState() {

  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: doorctrl panel, enter answering state");

  this->changeButtonText(this->activity_button, "Tür öffnen");
  g_signal_handler_disconnect(G_OBJECT(this->activity_button), this->activity_signal_id); 
  this->activity_signal_id = g_signal_connect(G_OBJECT(this->activity_button), "clicked", G_CALLBACK(doorctrl::UiControler::handleDoorOpenerButtonEvent), this->controler.get()); 

  gtk_widget_show(this->mute_button);
  gtk_widget_show(this->volume_button);
  gtk_widget_show(this->activity_button);
  gtk_widget_show(this->hangup_button);

  if(this->has_video_stream_error) {
     gtk_widget_show(this->error_subline);
  } else {
     gtk_widget_hide(this->error_subline);
  }

  gtk_stack_set_visible_child_name(GTK_STACK(this->state_indicator), "established");

}


void doorctrl::DoorControlPanel::enterConnectingState() {

  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: doorctrl panel, enter connecting state");
  gtk_widget_hide(this->mute_button);
  gtk_widget_hide(this->volume_button);
  gtk_widget_hide(this->activity_button);
  gtk_widget_hide(this->hangup_button);
  gtk_widget_hide(this->error_subline);
  gtk_stack_set_visible_child_name(GTK_STACK(this->state_indicator), "connecting");

}


void doorctrl::DoorControlPanel::enterVideoStreamErrorState() {

  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: doorctrl panel, enter video stream error state");

  this->has_video_stream_error = true;
  gtk_widget_show(this->error_subline);

}


void doorctrl::DoorControlPanel::enterVideoStreamNormalState() {

  doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: doorctrl panel, enter video stream normal state");

  this->has_video_stream_error = false;
  gtk_widget_hide(this->error_subline);

}


void doorctrl::DoorControlPanel::changeButtonText(GtkWidget* button, const gchar* text) {

  GtkWidget *button_label = gtk_bin_get_child(GTK_BIN(button)); 
  if(button_label != nullptr) {
     gtk_label_set_use_markup(GTK_LABEL(button_label), true);
     gtk_label_set_label(GTK_LABEL(button_label), text);
  }
}


void doorctrl::DoorControlPanel::updateHighlightColor(const std::string& highlight_color) {

   if(!highlight_color.empty()) {
      doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: doorctrl panel, update highlight color " +  highlight_color);

      std::string style = "* { color: " + highlight_color + "; }"; 
      std::string background_button_style = "* { background: " + highlight_color + "; }\n*:checked { background: " + highlight_color + "; }\n*:active { background: " + highlight_color + "; }"; 
      std::string checked_button_style = "*:checked { background: " + highlight_color + "; }\n*:active { background: " + highlight_color + "; }"; 

      doorctrl::GtkUiLogger::getInstance().getOutput().printDebug("Debug: doorctrl panel, style " +  style);
      gtk_css_provider_load_from_data(this->state_label_style_provider, style.c_str(), -1, NULL);


      gtk_css_provider_load_from_data(this->activity_button_style_provider, background_button_style.c_str(), -1, NULL);

      gtk_css_provider_load_from_data(this->mute_button_style_provider, checked_button_style.c_str(), -1, NULL);

      gtk_css_provider_load_from_data(this->volume_button_style_provider, checked_button_style.c_str(), -1, NULL);

      gtk_css_provider_load_from_data(this->hangup_button_style_provider, checked_button_style.c_str(), -1, NULL);
   }
}

