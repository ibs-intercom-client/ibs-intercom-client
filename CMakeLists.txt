cmake_minimum_required(VERSION 3.10)
project(doorctrl)

set(CMAKE_CXX_STANDARD 14)

find_package(PkgConfig REQUIRED)
link_directories(usr/lib/)
link_directories(/usr/lib/arm-linux-gnueabihf/)

add_compile_options(-Wall -pedantic -O2 --std=c++17 -DPJ_IS_LITTLE_ENDIAN=1 -DPJ_IS_BIG_ENDIAN=0)

include_directories(src)

pkg_check_modules(GTK3.0 REQUIRED gtk+-3.0)
pkg_check_modules(GLIB2.0 REQUIRED glib-2.0)
pkg_check_modules(GSTREAMER1.0 REQUIRED gstreamer-1.0)
pkg_check_modules(LIBPJPROJECT REQUIRED libpjproject)
pkg_check_modules(GSTREAMER_VIDEO REQUIRED gstreamer-video-1.0)
pkg_check_modules(PJSIP REQUIRED libpjproject )

add_library(common STATIC
        src/common/common_logger.cpp
        src/common/common_logger.h
        src/common/credentials.cpp
        src/common/credentials.h
        src/common/door_opener.h
        src/common/health_provider.h
        src/common/installation.h
        src/common/null_ringtone_player.cpp
        src/common/null_ringtone_player.h
        src/common/observed_subject.h
        src/common/phone_call_state.cpp
        src/common/phone_call_state.h
        src/common/phone_call_state_event.cpp
        src/common/phone_call_state_event.h
        src/common/phone_call_state_observer.h
        src/common/phone_event.h
        src/common/phone_exception.h
        src/common/phone.h
        src/common/phone_registration_state.cpp
        src/common/phone_registration_state.h
        src/common/phone_registration_state_event.cpp
        src/common/phone_registration_state_event.h
        src/common/phone_registration_state_observer.h
        src/common/phone_setup_exception.h
        src/common/phone_setup_exception.cpp
        src/common/push_button_state_event.cpp
        src/common/push_button_state_event.h
        src/common/push_button_state_observer.h
        src/common/home_button_state_event.cpp
        src/common/home_button_state_event.h
        src/common/home_button_state_observer.h
        src/common/ringtone_player_exception.h
        src/common/ringtone_player.h
        src/common/ringtone_player_setup_exception.cpp
        src/common/ringtone_player_setup_exception.h
        src/common/severity_level.h
        src/common/simple_observer_manager.cpp
        src/common/simple_observer_manager.h
        src/common/single_instance_lock.cpp
        src/common/single_instance_lock.h
        src/common/video_player.h src/common/home_button.h
        src/common/baselib_manager.cpp
        src/common/baselib_manager.h)

add_library(config STATIC
        src/config/application_config.cpp
        src/config/application_config.h
        src/config/config_data_changed_event.cpp
        src/config/config_data_changed_event.h
        src/config/config_data_changed_observer.h
        src/config/config_data.cpp
        src/config/config_data.h
        src/config/config_data_persistence.h
        src/config/config_data_event.h
        src/config/config_data_exception.cpp
        src/config/config_data_exception.h
        src/config/config_data_file_reader.cpp
        src/config/config_data_file_reader.h
        src/config/config_logger.cpp
        src/config/config_logger.h
        src/config/doorbell_config_data_factory.cpp
        src/config/doorbell_config_data_factory.h
        src/config/doorctrl_config_data_factory.cpp
        src/config/doorctrl_config_data_factory.h
        src/config/health_config.cpp
        src/config/health_config.h
        src/config/homegear_rpc_config.cpp
        src/config/homegear_rpc_config.h
        src/config/logging_config.cpp
        src/config/logging_config.h
        src/config/phone_config.cpp
        src/config/phone_config.h
        src/config/push_button_config.cpp
        src/config/push_button_config.h
        src/config/home_button_config.cpp
        src/config/home_button_config.h
        src/config/ringtone_config.cpp
        src/config/ringtone_config.cpp
        src/config/ringtone_config.h
        src/config/severity_level_helper.cpp
        src/config/severity_level_helper.h
        src/config/sip_config.cpp
        src/config/sip_config.h
        src/config/ui_config.cpp
        src/config/ui_config.h
        src/config/video_config.cpp
        src/config/video_config.h)

add_library(gpioconnector STATIC
        src/gpio_connector/gpio_connector_logger.cpp
        src/gpio_connector/gpio_connector_logger.h
        src/gpio_connector/home_button_connector.cpp
        src/gpio_connector/home_button_connector.h
        src/gpio_connector/push_button_connector.cpp
        src/gpio_connector/push_button_connector.h)


add_library(gstreamermedia STATIC
        src/gstreamer_media/gstreamer_media_logger.cpp
        src/gstreamer_media/gstreamer_media_logger.h
        src/gstreamer_media/gstreamer_ringtone_player.cpp
        src/gstreamer_media/gstreamer_ringtone_player.h)

target_compile_options(gstreamermedia PUBLIC ${GLIB2.0_CFLAGS} ${GLIB2.0_CFLAGS_OTHER})
target_compile_options(gstreamermedia PUBLIC ${GSTREAMER1.0_CFLAGS} ${GSTREAMER1.0_CFLAGS_OTHER})


add_library(gtkui STATIC
        src/gtk_ui/door_control_button_monkey.cpp
        src/gtk_ui/door_control_button_monkey.h
        src/gtk_ui/door_control_panel.cpp
        src/gtk_ui/door_control_panel.h
        src/gtk_ui/gtk_ui.cpp
        src/gtk_ui/gtk_ui.h
        src/gtk_ui/gtk_ui_logger.cpp
        src/gtk_ui/gtk_ui_logger.h
        src/gtk_ui/mainwindow.cpp
        src/gtk_ui/mainwindow.h
        src/gtk_ui/ui_controler.cpp
        src/gtk_ui/ui_controler.h
        src/gtk_ui/video_panel.cpp
        src/gtk_ui/video_panel.h
        src/gtk_ui/video_stream_state.cpp
        src/gtk_ui/video_stream_state.h
        src/gtk_ui/video_stream_state_event.cpp
        src/gtk_ui/video_stream_state_event.h
        src/gtk_ui/video_stream_state_observer.h
        src/gtk_ui/x_window_controler.cpp
        src/gtk_ui/x_window_controler.h)

target_compile_options(gtkui PUBLIC ${GTK3.0_CFLAGS} ${GTK3.0_CFLAGS_OTHER})
target_compile_options(gtkui PUBLIC ${GLIB2.0_CFLAGS} ${GLIB2.0_CFLAGS_OTHER})
target_compile_options(gtkui PUBLIC ${GSTREAMER1.0_CFLAGS} ${GSTREAMER1.0_CFLAGS_OTHER})


add_library(homegearconnector STATIC
        src/homegear_connector/intercom_config_data_connector.cpp
        src/homegear_connector/intercom_config_data_connector.h
        src/homegear_connector/doorbell_config_data_connector.cpp
        src/homegear_connector/doorbell_config_data_connector.h
        src/homegear_connector/homegear_connector_logger.cpp
        src/homegear_connector/homegear_connector_logger.h
        src/homegear_connector/rpc_client.cpp
        src/homegear_connector/rpc_client.h
        src/homegear_connector/rpc_client_exception.h
        src/homegear_connector/rpc_event_connector.cpp
        src/homegear_connector/rpc_event_connector.h)

target_include_directories(homegearconnector PUBLIC /usr/include/homegear-base)


add_library(httpinterface STATIC
        src/http_interface/basic_http_request_handler.cpp
        src/http_interface/basic_http_request_handler.h
        src/http_interface/health_http_request_handler.cpp
        src/http_interface/health_http_request_handler.h
        src/http_interface/http_interface_logger.cpp
        src/http_interface/http_interface_logger.h
        src/http_interface/http_request_handler.h
        src/http_interface/http_request_handler_manager.h
        src/http_interface/http_server.cpp
        src/http_interface/http_server.h)

add_library(pjsipphone STATIC
        src/pjsip_phone/account.cpp
        src/pjsip_phone/account.h
        src/pjsip_phone/call.cpp
        src/pjsip_phone/call.h
        src/pjsip_phone/pjsip_phone.cpp
        src/pjsip_phone/pjsip_phone.h
        src/pjsip_phone/pjsip_phone_logger.cpp
        src/pjsip_phone/pjsip_phone_logger.h)

add_executable(doorctrl
        src/doorctrl_appname.h
        src/doorctrl_command_line_handler.cpp
        src/doorctrl_command_line_handler.h
        src/doorctrl_command_line_options.h
        src/doorctrl.cpp
        src/doorctrl.h
        src/doorctrl_exception.cpp
        src/doorctrl_exception.h
        src/doorctrl_logger.cpp
        src/doorctrl_logger.h
        src/doorctrl_version.h
        src/health_collector.cpp
        src/health_collector.h
        src/reboot_handler.cpp
        src/reboot_handler.h)

add_executable(doorbell
        src/doorbell_appname.h
        src/doorbell_command_line_handler.cpp
        src/doorbell_command_line_handler.h
        src/doorbell_command_line_options.h
        src/doorbell.cpp
        src/doorbell.h
        src/doorbell_exception.cpp
        src/doorbell_exception.h
        src/doorbell_logger.cpp
        src/doorbell_logger.h
        src/doorbell_version.h
        src/health_collector.cpp
        src/health_collector.h)


target_link_libraries(doorctrl gtkui)
target_link_libraries(doorctrl pjsipphone)
target_link_libraries(doorctrl gstreamermedia)
target_link_libraries(doorctrl homegearconnector)
target_link_libraries(doorctrl httpinterface)
target_link_libraries(doorctrl gpioconnector)
target_link_libraries(doorctrl config)
target_link_libraries(doorctrl common)
target_link_libraries(doorctrl homegear-base)
target_link_libraries(doorctrl gnutls)
target_link_libraries(doorctrl X11)
target_link_libraries(doorctrl Xext)
target_link_libraries(doorctrl Xss)
target_link_libraries(doorctrl Xtst)
target_link_libraries(doorctrl gtk-3 gdk-3 gdk_pixbuf-2.0 gio-2.0 gobject-2.0 glib-2.0)
target_link_libraries(doorctrl gstreamer-1.0 gstvideo-1.0)
#target_link_libraries(doorctrl ${GSTREAMER1.0_LIBRARIES})
target_link_libraries(doorctrl ${GSTREAMER_VIDEO_LIBRARIES})
#target_link_libraries(doorctrl ${LIBPJPROJECT_LIBRARIES})
target_link_libraries(doorctrl ${PJSIP_LIBRARIES})
target_link_libraries(doorctrl speex-armv7l-unknown-linux-gnueabihf webrtc-armv7l-unknown-linux-gnueabihf srtp-armv7l-unknown-linux-gnueabihf gsmcodec-armv7l-unknown-linux-gnueabihf ilbccodec-armv7l-unknown-linux-gnueabihf resample-armv7l-unknown-linux-gnueabihf)
target_link_libraries(doorctrl asound)
target_link_libraries(doorctrl uuid)
target_link_libraries(doorctrl ssl crypto)
target_link_libraries(doorctrl pthread)

target_link_libraries(doorbell gstreamermedia)
target_link_libraries(doorbell homegearconnector)
target_link_libraries(doorbell httpinterface)
target_link_libraries(doorbell gpioconnector)
target_link_libraries(doorbell config)
target_link_libraries(doorbell common)
target_link_libraries(doorbell homegear-base)
target_link_libraries(doorbell gnutls)
target_link_libraries(doorbell gcrypt)
target_link_libraries(doorbell gpiod)
target_link_libraries(doorbell gstreamer-1.0 gstvideo-1.0)
target_link_libraries(doorbell gobject-2.0 glib-2.0)
target_link_libraries(doorbell pthread)

install(TARGETS doorctrl doorbell RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
