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

#define BOOST_TEST_MODULE config_tests 
#include <boost/test/included/unit_test.hpp>
#include <config/config_data_file_reader.h>
#include "config/sip_config.h"
#include "config/ringtone_config.h"
#include "config/video_config.h"
#include "config/logging_config.h"
#include "config/phone_config.h"
#include "config/application_config.h"
#include "config/health_config.h"
#include "config/config_data.h"
#include "config/config_data_changed_observer.h"
#include "common/severity_level.h"

class TestConfigDataChangedObserver : public doorctrl::ConfigDataChangedObserver {
  public:
    TestConfigDataChangedObserver() : last("undefined") {};
    ~TestConfigDataChangedObserver() {};
    void handleEvent(const doorctrl::ConfigDataChangedEvent& event) override {this->last = event;};
    doorctrl::ConfigDataChangedEvent last;
};



BOOST_AUTO_TEST_SUITE(sip_config)

BOOST_AUTO_TEST_CASE(check_initial_sip_config_values) {

  doorctrl::SipConfig sip_config;
  BOOST_TEST(sip_config.registrar == "");
  BOOST_TEST(sip_config.udp_port == 5060);
  BOOST_TEST(sip_config.reg_interval == 300);
  BOOST_TEST(sip_config.credentials.username == "");
  BOOST_TEST(sip_config.credentials.password == "");
  BOOST_TEST(sip_config.display_name == "");
  BOOST_TEST(sip_config.ec_enabled == true);
  BOOST_TEST(sip_config.ec_noise_suppressor_enabled == true);
  BOOST_TEST(sip_config.ec_tail_len == 500);
  BOOST_TEST(sip_config.ec_aggressiveness == 0);
}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(ringtone_config)

BOOST_AUTO_TEST_CASE(check_initial_ringtone_config_values) {

  doorctrl::RingtoneConfig ringtone_config;
  BOOST_TEST(ringtone_config.path == "" );
  BOOST_TEST(ringtone_config.volume == 100);
  BOOST_TEST(ringtone_config.mute == false);
  BOOST_TEST(ringtone_config.duration == 2);
}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(video_config)

BOOST_AUTO_TEST_CASE(check_initial_video_config_values) {

  doorctrl::VideoConfig video_config;
  BOOST_TEST(video_config.video_url == "" );
  BOOST_TEST(video_config.fade_out_seconds == 0);
  BOOST_TEST(video_config.enabled == true);
}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(logging_config)

BOOST_AUTO_TEST_CASE(check_initial_logging_config_values) {

  doorctrl::LoggingConfig logging_config;
  BOOST_TEST(logging_config.logdir == "/var/log/doorctrl/");
  BOOST_TEST(logging_config.logfile_prefix == "doorctrl");
  BOOST_TEST(logging_config.loglevel == doorctrl::severity_level::error);
  BOOST_TEST(logging_config.logging_disabled == false);
  BOOST_TEST(logging_config.rotation_size == 10 * 1024 * 1024);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(phone_config)

BOOST_AUTO_TEST_CASE(check_initial_phone_config_values) {

  doorctrl::PhoneConfig phone_config;
  BOOST_TEST(phone_config.volume == 100 );
  BOOST_TEST(phone_config.mic_level == 100);
  BOOST_TEST(phone_config.mute_mic == false);
}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(application_config)

BOOST_AUTO_TEST_CASE(check_initial_application_config_values) {

  doorctrl::ApplicationConfig application_config;
  BOOST_TEST(application_config.app_version == "" );
  BOOST_TEST(application_config.app_name == "");
  BOOST_TEST(application_config.binary_name == "");
  BOOST_TEST(application_config.kiosk_mode == false);
  BOOST_TEST(application_config.argc_ptr == nullptr);
  BOOST_TEST(application_config.argv_ptr == nullptr);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(homegear_rpc_config)

BOOST_AUTO_TEST_CASE(check_initial_homegear_rpc_config_values) {

  doorctrl::HomegearRpcConfig rpc_config;
  BOOST_TEST(rpc_config.enabled == false);
  BOOST_TEST(rpc_config.address == "");
  BOOST_TEST(rpc_config.port == 2001);
  BOOST_TEST(rpc_config.intercom_device_id == "Intercom");
  BOOST_TEST(rpc_config.doorbell_device_id == "Doorbell");
  BOOST_TEST(rpc_config.home_button_device_id == "Homebutton");
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(homebutton_config)

BOOST_AUTO_TEST_CASE(check_initial_homebutton_config_values) {

   doorctrl::HomeButtonConfig homebutton_config;
   BOOST_TEST(homebutton_config.gpio_input == 255);
   BOOST_TEST(homebutton_config.inverted == false);
   BOOST_TEST(homebutton_config.debounce == true);
   BOOST_TEST(homebutton_config.long_push_duration == 20);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(pushbutton_config)

BOOST_AUTO_TEST_CASE(check_initial_pushbutton_config_values) {
  doorctrl::PushButtonConfig pushbutton_config;
  BOOST_TEST(pushbutton_config.gpio_input == 255);
}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(health_config)

BOOST_AUTO_TEST_CASE(check_initial_health_config_values) {

  doorctrl::HealthConfig health_config;
  BOOST_TEST(health_config.enabled == false);
  BOOST_TEST(health_config.http_bind_address == "");
  BOOST_TEST(health_config.http_port == 0);
  BOOST_TEST(health_config.health_collector_sleep == 60);
}

BOOST_AUTO_TEST_SUITE_END()




BOOST_AUTO_TEST_SUITE(config_data)

BOOST_AUTO_TEST_CASE(update_sip_data) {

  doorctrl::ConfigData config_data;
  doorctrl::SipConfig sip_config;
  
  sip_config.registrar = "Hallo, nix wie weg"; 
  sip_config.udp_port = 0xFFFF - 1; 

  config_data.updateSipConfig(sip_config);

  BOOST_TEST(config_data.getSipConfig().registrar == sip_config.registrar);
  BOOST_TEST(config_data.getSipConfig().udp_port == sip_config.udp_port);
  BOOST_TEST(config_data.getSipConfig().udp_port == 0xFFFF - 1);
}

BOOST_AUTO_TEST_CASE(check_update_sip_config_changed_event) {

  std::shared_ptr<TestConfigDataChangedObserver> observer(new TestConfigDataChangedObserver()); 
  doorctrl::ConfigData config_data;
  doorctrl::SipConfig sip_config;
  
  sip_config.registrar = "Hallo, nix wie weg"; 
  sip_config.udp_port = 0xFFFF - 1; 

  config_data.addObserver(observer);
  config_data.updateSipConfig(sip_config);

  BOOST_TEST(observer->last.getChangedDataSection() == doorctrl::ConfigDataChangedEvent::SIP_SECTION_NAME);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(config_file_reader)

BOOST_AUTO_TEST_CASE(read_test_file) {
    auto config_data = std::make_shared<doorctrl::ConfigData>();
    doorctrl::ConfigDataFileReader::readConfigData(config_data, "../doorctrl_test1.conf", "doorctrl_test1.conf");

    doorctrl::SipConfig sip_config = config_data->getSipConfig();
    BOOST_TEST(sip_config.registrar == "192.168.200.153");
    BOOST_TEST(sip_config.udp_port == 5060);
    BOOST_TEST(sip_config.reg_interval == 300);
    BOOST_TEST(sip_config.credentials.username == "whg1");
    BOOST_TEST(sip_config.credentials.password == "fJ7D§otv!");
    BOOST_TEST(sip_config.display_name == "Wohnung1");
    BOOST_TEST(sip_config.ec_enabled == true);
    BOOST_TEST(sip_config.ec_noise_suppressor_enabled == true);
    BOOST_TEST(sip_config.ec_tail_len == 550);
    BOOST_TEST(sip_config.ec_aggressiveness == 3);

    doorctrl::RingtoneConfig ringtone_config = config_data->getRingtoneConfig();
    BOOST_TEST(ringtone_config.path == "/usr/share/doorctrl/sounds/178137__keybit2__ring-ring.mp3");
    BOOST_TEST(ringtone_config.volume == 75);
    BOOST_TEST(ringtone_config.mute == false);
    BOOST_TEST(ringtone_config.duration == 6);

    doorctrl::PhoneConfig phone_config = config_data->getPhoneConfig();
    BOOST_TEST(phone_config.volume == 97);
    BOOST_TEST(phone_config.mic_level == 77);
    BOOST_TEST(phone_config.mute_mic == false);

   doorctrl::VideoConfig video_config = config_data->getVideoConfig();
   BOOST_TEST(video_config.video_url == "http://192.168.200.195/camera/stream?width=640&height=480");
   BOOST_TEST(video_config.enabled == true);

   doorctrl::LoggingConfig logging_config = config_data->getLoggingConfig();
   BOOST_TEST(logging_config.logdir == "/var/log/doorctrl/");
   BOOST_TEST(logging_config.logfile_prefix == "doorctrl");
   BOOST_TEST(logging_config.loglevel == doorctrl::severity_level::info);
   BOOST_TEST(logging_config.logging_disabled == false);
   BOOST_TEST(logging_config.rotation_size == 1000000);

   doorctrl::HealthConfig health_config = config_data->getHealthConfig();
   BOOST_TEST(health_config.enabled == true);
   BOOST_TEST(health_config.http_bind_address == "192.168.200.195");
   BOOST_TEST(health_config.http_port == 9541);
   BOOST_TEST(health_config.health_collector_sleep == 60);

  doorctrl::HomegearRpcConfig rpc_config = config_data->getHomegearRpcConfig();
  BOOST_TEST(rpc_config.enabled == false);
  BOOST_TEST(rpc_config.address == "10.130.0.1");
  BOOST_TEST(rpc_config.port == 2001);
  BOOST_TEST(rpc_config.intercom_device_id == "Intercom_Test_Device");
  BOOST_TEST(rpc_config.doorbell_device_id == "Doorbell_Test_Device");
  BOOST_TEST(rpc_config.home_button_device_id == "Homebutton_Test_Device");

  doorctrl::HomeButtonConfig homebutton_config = config_data->getHomeButtonConfig();
  BOOST_TEST(homebutton_config.gpio_input == 29);
  BOOST_TEST(homebutton_config.inverted == false);
  BOOST_TEST(homebutton_config.debounce == true);
  BOOST_TEST(homebutton_config.long_push_duration == 23);

  doorctrl::PushButtonConfig pushbutton_config = config_data->getPushButtonConfig();
  BOOST_TEST(pushbutton_config.gpio_input == 30);
}

BOOST_AUTO_TEST_CASE(test_reader_default_values) {
  auto config_data = std::make_shared<doorctrl::ConfigData>();
  doorctrl::ConfigDataFileReader::readConfigData(config_data, "../doorctrl_minimal.conf", "doorctrl_minimal.conf");

  doorctrl::SipConfig sip_config = config_data->getSipConfig();
  BOOST_TEST(sip_config.registrar == "");
  BOOST_TEST(sip_config.udp_port == 5060);
  BOOST_TEST(sip_config.reg_interval == 300);
  BOOST_TEST(sip_config.credentials.username == "");
  BOOST_TEST(sip_config.credentials.password == "");
  BOOST_TEST(sip_config.display_name == "");
  BOOST_TEST(sip_config.ec_enabled == true);
  BOOST_TEST(sip_config.ec_noise_suppressor_enabled == true);
  BOOST_TEST(sip_config.ec_tail_len == 500);
  BOOST_TEST(sip_config.ec_aggressiveness == 0);

  doorctrl::RingtoneConfig ringtone_config = config_data->getRingtoneConfig();
  BOOST_TEST(ringtone_config.path == "");
  BOOST_TEST(ringtone_config.volume == 100);
  BOOST_TEST(ringtone_config.mute == false);
  BOOST_TEST(ringtone_config.duration == 2);

  doorctrl::PhoneConfig phone_config = config_data->getPhoneConfig();
  BOOST_TEST(phone_config.volume == 100);
  BOOST_TEST(phone_config.mic_level == 100);
  BOOST_TEST(phone_config.mute_mic == false);

  doorctrl::VideoConfig video_config = config_data->getVideoConfig();
  BOOST_TEST(video_config.video_url == "");
  BOOST_TEST(video_config.enabled == true);

  doorctrl::LoggingConfig logging_config = config_data->getLoggingConfig();
  BOOST_TEST(logging_config.logdir == "/var/log/doorctrl/");
  BOOST_TEST(logging_config.logfile_prefix == "doorctrl");
  BOOST_TEST(logging_config.loglevel == doorctrl::severity_level::error);
  BOOST_TEST(logging_config.logging_disabled == false);
  BOOST_TEST(logging_config.rotation_size == 10 * 1024 * 1024);

  doorctrl::HealthConfig health_config = config_data->getHealthConfig();
  BOOST_TEST(health_config.enabled == false);
  BOOST_TEST(health_config.http_bind_address == "127.0.0.1");
  BOOST_TEST(health_config.http_port == (uint16_t)9441);
  BOOST_TEST(health_config.health_collector_sleep == 60);

  doorctrl::HomegearRpcConfig rpc_config = config_data->getHomegearRpcConfig();
  BOOST_TEST(rpc_config.enabled == false);
  BOOST_TEST(rpc_config.address == "127.0.0.1");
  BOOST_TEST(rpc_config.port == (uint16_t)2001);
  BOOST_TEST(rpc_config.intercom_device_id == "Intercom");
  BOOST_TEST(rpc_config.doorbell_device_id == "Doorbell");
  BOOST_TEST(rpc_config.home_button_device_id == "Homebutton");

  doorctrl::HomeButtonConfig homebutton_config = config_data->getHomeButtonConfig();
  BOOST_TEST(homebutton_config.gpio_input == 255);
  BOOST_TEST(homebutton_config.inverted == false);
  BOOST_TEST(homebutton_config.debounce == true);
  BOOST_TEST(homebutton_config.long_push_duration == 20);

  doorctrl::PushButtonConfig pushbutton_config = config_data->getPushButtonConfig();
  BOOST_TEST(pushbutton_config.gpio_input == 255);
 }

BOOST_AUTO_TEST_SUITE_END()
