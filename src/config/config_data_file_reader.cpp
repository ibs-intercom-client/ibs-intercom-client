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

#include "config_data_file_reader.h"
#include "config_data.h"
#include "config_logger.h"
#include "config_data_exception.h"
#include "severity_level_helper.h"
#include <string>
#include <fstream>
#include <iostream>
#include <homegear-base/HelperFunctions/Io.h>

void readSipConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections, 
                   std::shared_ptr<doorctrl::ConfigData>& config_data);
void readRingtoneConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections, 
                        std::shared_ptr<doorctrl::ConfigData>& config_data);
void readVideoConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections, 
                     std::shared_ptr<doorctrl::ConfigData>& config_data);
void readLoggingConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections, 
                       std::shared_ptr<doorctrl::ConfigData>& config_data);
void readPhoneConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections, 
                     std::shared_ptr<doorctrl::ConfigData>& config_data);
void readPushButtonConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections, 
                          std::shared_ptr<doorctrl::ConfigData>& config_data);
void readHomeButtonConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections, 
                          std::shared_ptr<doorctrl::ConfigData>& config_data);
void readHealthConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections, 
                      std::shared_ptr<doorctrl::ConfigData>& config_data);
void readHomegearRpcConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections, 
                           std::shared_ptr<doorctrl::ConfigData>& config_data);
void readUiConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections, 
                  std::shared_ptr<doorctrl::ConfigData>& config_data);


void doorctrl::ConfigDataFileReader::readConfigData(std::shared_ptr<doorctrl::ConfigData>& config_data, const std::string& global_file_path, const std::string& user_file_name) {
  std::string current_file = "<undefined>";
  char* homedir = getenv("HOME");

    /*
     * do some file checks here to find a version of the doorctrl config 
     * file. The user defined config file is used in favour. 
     * An error will be thrown when there is no config file at all, in case
     * of an format error of when the file is deleted before is has been
     * read completely (this should never happen in real life).
     */ 
    if(homedir != nullptr && BaseLib::Io::fileExists(std::string(homedir) + "/" + user_file_name)) {
      current_file = std::string(homedir) + "/" + user_file_name;
      doorctrl::ConfigLogger::getInstance().getOutput().printDebug("Debug: reading config file '" +  current_file + "'");    
    } else if(BaseLib::Io::fileExists(global_file_path)) {
      current_file = global_file_path;
      doorctrl::ConfigLogger::getInstance().getOutput().printDebug("Debug: reading config file '" +  global_file_path +  "'");    
    } else {
      doorctrl::ConfigLogger::getInstance().getOutput().printError("Error: config file not found, provide either '" + global_file_path + "' or '$HOME/" +  user_file_name + "'");    
      throw ConfigDataException("config file not found, supply either '" + global_file_path + "' or '~/" + user_file_name  + "'");
    }

    std::ifstream config_stream(current_file);
    int line_counter = 0;
    std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>> sections; 
    std::map<std::string, std::string>* current_section = nullptr;

    if(config_stream.is_open()) {

      std::string line;
      while(getline(config_stream, line)) {

        line_counter++;

        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
        auto comment_pos = line.find("#"); 

        if(comment_pos != std::string::npos) {
          line.erase(comment_pos);
        }

        if(line.empty()) {
          continue;
        } else if(line[0] == '[') {
           auto closing_pos = line.find("]"); 

           if(closing_pos != std::string::npos) {
              if(closing_pos == line.length() - 1) { 
                 std::string section_name = line.substr(1, closing_pos - 1); 

                 std::shared_ptr<std::map<std::string, std::string>> new_section = std::make_shared<std::map<std::string, std::string>>(); 
                 current_section = new_section.get();
                 sections.insert(std::pair<std::string, std::shared_ptr<std::map<std::string, std::string>>>(section_name, std::move(new_section)));
                 doorctrl::ConfigLogger::getInstance().getOutput().printDebug("Debug: section '" + section_name + "' found in line " + std::to_string(line_counter));    

              } else {
                 doorctrl::ConfigLogger::getInstance().getOutput().printError("Error: trailing characters in config file '" + current_file + "' at line " + std::to_string(line_counter));    
            
                 config_stream.close();  
                 throw ConfigDataException("format error in config file '" + current_file + "'");
              }
           } else {
              doorctrl::ConfigLogger::getInstance().getOutput().printError("Error: missing closing ']' in config file '" + current_file + "' at line " + std::to_string(line_counter));    
            
              config_stream.close();  
              throw ConfigDataException("format error in config file '" + current_file + "'");
           }
        } else {
           auto delim_pos = line.find("="); 

           if(delim_pos != std::string::npos) {
              if(delim_pos == 0) {
                 doorctrl::ConfigLogger::getInstance().getOutput().printError("Error: missing property name '=' in config file '" + current_file + "' at line " + std::to_string(line_counter));    
            
                 config_stream.close();  
                 throw ConfigDataException("format error in config file '" + current_file + "'");
              }

              std::string name = line.substr(0, delim_pos); 
              std::string value = line.substr(delim_pos + 1); 

              current_section->insert(std::pair<std::string, std::string>(name, value));
              doorctrl::ConfigLogger::getInstance().getOutput().printDebug("Debug: option '" + name + "' = '" + value + "' found in line " + std::to_string(line_counter));    

           } else {
              doorctrl::ConfigLogger::getInstance().getOutput().printError("Error: missing delimiter '=' in config file '" + current_file + "' at line " + std::to_string(line_counter));    
            
              config_stream.close();  
              throw ConfigDataException("format error in config file '" + current_file + "'");
           }
        }
      }
    }
  
    readSipConfig(sections, config_data);
    readRingtoneConfig(sections, config_data);
    readVideoConfig(sections, config_data);
    readLoggingConfig(sections, config_data);
    readPhoneConfig(sections, config_data);
    readPushButtonConfig(sections, config_data);
    readHomeButtonConfig(sections, config_data);
    readHealthConfig(sections, config_data);
    readHomegearRpcConfig(sections, config_data);
    readUiConfig(sections, config_data);
}

/*
 * Try to find the properties of section '[sip]' which have been read from configuration file
 * If there is no such section or if a single value is missing the a default
 * value is inserted into the configuration data.
 */
void readSipConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections, 
                   std::shared_ptr<doorctrl::ConfigData>& config_data) {

  auto it = sections.find("sip");
  std::shared_ptr<std::map<std::string, std::string>> sip_properties;

   if(it == sections.end()) {
     sip_properties = std::make_shared<std::map<std::string, std::string>>();
  } else {
     sip_properties = it->second;
  }

  doorctrl::SipConfig sip_config;

  auto prit = sip_properties->find("registrar");
  if(prit != sip_properties->end()) {
     sip_config.registrar = prit->second; 
  }

  prit = sip_properties->find("udp_port");
  if(prit != sip_properties->end() && !prit->second.empty()) {
     sip_config.udp_port = atoi(prit->second.c_str()); 
  } else {
     sip_config.udp_port = 0;
  }

   prit = sip_properties->find("audio_port_start");
   if(prit != sip_properties->end() && !prit->second.empty()) {
      sip_config.audio_port_start = atoi(prit->second.c_str());
   } else {
      sip_config.audio_port_start = 10000;
   }

   prit = sip_properties->find("audio_port_range");
  if(prit != sip_properties->end() && !prit->second.empty()) {
     sip_config.audio_port_range = atoi(prit->second.c_str());
  } else {
     sip_config.audio_port_range = 10000;
  }

   prit = sip_properties->find("audio_bind_address");
   if(prit != sip_properties->end() && !prit->second.empty()) {
      sip_config.audio_bind_address = prit->second;
   }

  prit = sip_properties->find("reg_interval");
  if(prit != sip_properties->end() && !prit->second.empty()) {
     sip_config.reg_interval = atoi(prit->second.c_str()); 
  } else {
     sip_config.reg_interval = 300;
  }

  prit = sip_properties->find("username");
  if(prit != sip_properties->end()) {
     sip_config.credentials.username = prit->second; 
  }

  prit = sip_properties->find("password");
  if(prit != sip_properties->end()) {
     sip_config.credentials.password = prit->second; 
  }

  prit = sip_properties->find("display_name");
  if(prit != sip_properties->end()) {
     sip_config.display_name = prit->second; 
  }

  prit = sip_properties->find("ec_enabled");
  if(prit != sip_properties->end() && !prit->second.empty()) {
     sip_config.ec_enabled = (prit->second == "1" || prit->second == "true"); 
  } else {
     sip_config.ec_enabled = true; 
  }

  prit = sip_properties->find("ec_noise_suppressor_enabled");
  if(prit != sip_properties->end() && !prit->second.empty()) {
     sip_config.ec_noise_suppressor_enabled = (prit->second == "1" || prit->second == "true"); 
  } else {
     sip_config.ec_noise_suppressor_enabled = true; 
  }

  prit = sip_properties->find("ec_tail_len");
  if(prit != sip_properties->end() && !prit->second.empty()) {
     sip_config.ec_tail_len = atoi(prit->second.c_str());
  } else {
     sip_config.ec_tail_len = (uint16_t)500;
  }

  prit = sip_properties->find("ec_aggressiveness");
  if(prit != sip_properties->end() && !prit->second.empty()) {
     sip_config.ec_aggressiveness = atoi(prit->second.c_str()); 
  } else {
     sip_config.ec_aggressiveness = 0;
  }

  config_data->updateSipConfig(sip_config);
}


/*
 * Try to find the properties of section '[ringtone]' which have been read from configuration file
 * If there is no such section or if a single value is missing the a default
 * value is inserted into the configuration data.
 */
void readRingtoneConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections,
                        std::shared_ptr<doorctrl::ConfigData>& config_data) {

  auto it = sections.find("ringtone");
  std::shared_ptr<std::map<std::string, std::string>> ringtone_properties;

  if(it == sections.end()) {
     ringtone_properties = std::make_shared<std::map<std::string, std::string>>();
  } else {
     ringtone_properties = it->second;
  }

  doorctrl::RingtoneConfig ringtone_config;

  auto prit = ringtone_properties->find("path");
  if(prit != ringtone_properties->end()) {
     ringtone_config.path = prit->second; 
  }

  prit = ringtone_properties->find("volume");
  if(prit != ringtone_properties->end() && !prit->second.empty()) {
     ringtone_config.volume = atoi(prit->second.c_str()); 
  } else {
     ringtone_config.volume = 100;
  }

  prit = ringtone_properties->find("mute");
  if(prit != ringtone_properties->end() && !prit->second.empty()) {
     ringtone_config.mute = (prit->second == "1" || prit->second == "true"); 
  } else {
     ringtone_config.mute = false; 
  }

  prit = ringtone_properties->find("duration");
  if(prit != ringtone_properties->end() && !prit->second.empty()) {
     ringtone_config.duration = atoi(prit->second.c_str()); 
  } else {
     ringtone_config.duration = 2;
  }

  config_data->updateRingtoneConfig(ringtone_config);
}


/*
 * Try to find the properties of section '[video]' which have been read from configuration file
 * If there is no such section or if a single value is missing the a default
 * value is inserted into the configuration data.
 */
void readVideoConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections,
                     std::shared_ptr<doorctrl::ConfigData>& config_data) {

  auto it = sections.find("video");
  std::shared_ptr<std::map<std::string, std::string>> video_properties;

   if(it == sections.end()) {
     video_properties = std::make_shared<std::map<std::string, std::string>>();
  } else {
     video_properties = it->second;
  }

  doorctrl::VideoConfig video_config;

  auto prit = video_properties->find("video-url");
  if(prit != video_properties->end()) {
     video_config.video_url = prit->second; 
  }


  prit = video_properties->find("enabled");
  if(prit != video_properties->end() && !prit->second.empty()) {
     video_config.enabled = (prit->second == "1" || prit->second == "true"); 
  } else {
     video_config.enabled = true; 
  }

  config_data->updateVideoConfig(video_config);
}


/*
 * Try to find the properties of section '[logging]' which have been read from configuration file
 * If there is no such section or if a single value is missing the a default
 * value is inserted into the configuration data.
 */
void readLoggingConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections,
                       std::shared_ptr<doorctrl::ConfigData>& config_data) {

  auto it = sections.find("logging");
  std::shared_ptr<std::map<std::string, std::string>> logging_properties;

  if(it == sections.end()) {
     logging_properties = std::make_shared<std::map<std::string, std::string>>();
  } else {
     logging_properties = it->second;
  }

  doorctrl::LoggingConfig logging_config;

  auto prit = logging_properties->find("logdir");
  if(prit != logging_properties->end() && !prit->second.empty() ) {
     logging_config.logdir = prit->second; 
  } else {
     logging_config.logdir = "/var/log/doorctrl/";
  }

  prit = logging_properties->find("file_prefix");
  if(prit != logging_properties->end()) {
     logging_config.logfile_prefix = prit->second; 
  } else {
     logging_config.logfile_prefix = "doorctrl"; 
  }

  prit = logging_properties->find("rotation-size");
  if(prit != logging_properties->end() && !prit->second.empty()) {
     logging_config.rotation_size = atoi(prit->second.c_str()); 
  } else {
     logging_config.rotation_size = 10 * 1024 * 1024;
  }

  int loglevel = 2;
  prit = logging_properties->find("loglevel");
  if(prit != logging_properties->end() && !prit->second.empty()) {
     loglevel = atoi(prit->second.c_str()); 
  } 

  doorctrl::SeverityLevelHelper helper(loglevel); 
  logging_config.loglevel = helper.getSeverityLevel(); 
  logging_config.logging_disabled = helper.isLoggingDisabled(); 

  config_data->updateLoggingConfig(logging_config);
}


/*
 * Try to find the properties of section '[phone]' which have been read from configuration file
 * If there is no such section or if a single value is missing the a default
 * value is inserted into the configuration data.
 */
void readPhoneConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections,
                     std::shared_ptr<doorctrl::ConfigData>& config_data) {

  auto it = sections.find("phone");
  std::shared_ptr<std::map<std::string, std::string>> phone_properties;

  if(it == sections.end()) {
     phone_properties = std::make_shared<std::map<std::string, std::string>>();
  } else {
     phone_properties = it->second;
  }

  doorctrl::PhoneConfig phone_config;

  auto prit = phone_properties->find("volume");
  if(prit != phone_properties->end() && !prit->second.empty()) {
     phone_config.volume = atoi(prit->second.c_str()); 
  } else {
     phone_config.volume = 100;
  }

  prit = phone_properties->find("mic-level");
  if(prit != phone_properties->end() && !prit->second.empty()) {
     phone_config.mic_level = atoi(prit->second.c_str()); 
  } else {
     phone_config.mic_level = 100;
  }

  prit = phone_properties->find("mute_mic");
  if(prit != phone_properties->end() && !prit->second.empty()) {
     phone_config.mute_mic = (prit->second == "1" || prit->second == "true"); 
  } else {
     phone_config.mute_mic = false; 
  }

  config_data->updatePhoneConfig(phone_config);
}


/*
 * Try to find the properties of section '[push_button]' which have been read from configuration file
 * If there is no such section or if a single value is missing the a default
 * value is inserted into the configuration data.
 */
void readPushButtonConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections,
                          std::shared_ptr<doorctrl::ConfigData>& config_data) {

  auto it = sections.find("push_button");
  std::shared_ptr<std::map<std::string, std::string>> push_button_properties;

  if(it == sections.end()) {
     push_button_properties = std::make_shared<std::map<std::string, std::string>>();
  } else {
     push_button_properties = it->second;
  }

  doorctrl::ConfigLogger::getInstance().getOutput().printDebug("Debug: Found section push_button");

  doorctrl::PushButtonConfig push_button_config;

  auto prit = push_button_properties->find("inverted");
  if(prit != push_button_properties->end() && !prit->second.empty()) {
     push_button_config.inverted = (prit->second == "1" || prit->second == "true"); 
  } else {
     push_button_config.inverted = false; 
  }

  prit = push_button_properties->find("gpio_input");
  if(prit != push_button_properties->end() && !prit->second.empty()) {
     push_button_config.gpio_input = atoi(prit->second.c_str()); 
  } else {
     push_button_config.gpio_input = 255;
  }
  doorctrl::ConfigLogger::getInstance().getOutput().printDebug("Debug: gpio_input set to " + std::to_string(push_button_config.gpio_input));

  prit = push_button_properties->find("debounce");
  if(prit != push_button_properties->end() && !prit->second.empty()) {
     push_button_config.debounce = (prit->second == "1" || prit->second == "true"); 
  } else {
     push_button_config.debounce = true; 
  }

  config_data->updatePushButtonConfig(push_button_config);
}


/*
 * Try to find the properties of section '[home_button]' which have been read from configuration file
 * If there is no such section or if a single value is missing the a default
 * value is inserted into the configuration data.
 */
void readHomeButtonConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections,
                          std::shared_ptr<doorctrl::ConfigData>& config_data) {

  auto it = sections.find("home_button");
  std::shared_ptr<std::map<std::string, std::string>> home_button_properties;

  if(it == sections.end()) {
     home_button_properties = std::make_shared<std::map<std::string, std::string>>();
  } else {
     home_button_properties = it->second;
  }

  doorctrl::ConfigLogger::getInstance().getOutput().printDebug("Debug: Found section home_button");

  doorctrl::HomeButtonConfig home_button_config;

  auto prit = home_button_properties->find("inverted");
  if(prit != home_button_properties->end() && !prit->second.empty()) {
     home_button_config.inverted = (prit->second == "1" || prit->second == "true"); 
  } else {
     home_button_config.inverted = false; 
  }

  prit = home_button_properties->find("gpio_input");
  if(prit != home_button_properties->end() && !prit->second.empty()) {
     home_button_config.gpio_input = atoi(prit->second.c_str()); 
  } else {
     home_button_config.gpio_input = 255;
  }
  doorctrl::ConfigLogger::getInstance().getOutput().printDebug("Debug: gpio_input set to " + std::to_string(home_button_config.gpio_input));

  prit = home_button_properties->find("debounce");
  if(prit != home_button_properties->end() && !prit->second.empty()) {
     home_button_config.debounce = (prit->second == "1" || prit->second == "true"); 
  } else {
     home_button_config.debounce = true; 
  }


   prit = home_button_properties->find("long_push_duration");
   if(prit != home_button_properties->end() && !prit->second.empty()) {
      home_button_config.long_push_duration = atoi(prit->second.c_str());;
   } else {
      home_button_config.long_push_duration = 20;
   }

  config_data->updateHomeButtonConfig(home_button_config);
}


/*
 * Try to find the properties of section '[health]' which have been read from configuration file
 * If there is no such section or if a single value is missing the a default
 * value is inserted into the configuration data.
 */
void readHealthConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections,
                      std::shared_ptr<doorctrl::ConfigData>& config_data) {

  auto it = sections.find("health");
  std::shared_ptr<std::map<std::string, std::string>> health_properties;

  if(it == sections.end()) {
     health_properties = std::make_shared<std::map<std::string, std::string>>();
  } else {
     health_properties = it->second;
  }

  doorctrl::HealthConfig health_config;

  auto prit = health_properties->find("enabled");
  if(prit != health_properties->end() && !prit->second.empty()) {
     health_config.enabled = (prit->second == "1" || prit->second == "true"); 
  } else {
     health_config.enabled = false; 
  }

  prit = health_properties->find("http_bind_address");
  if(prit != health_properties->end() && !prit->second.empty()) {
     health_config.http_bind_address = prit->second.c_str(); 
  } else {
     health_config.http_bind_address = "127.0.0.1";
  }

  prit = health_properties->find("http_port");
  if(prit != health_properties->end() && !prit->second.empty()) {
     health_config.http_port = atoi(prit->second.c_str());
  } else {
     health_config.http_port = 9441;
  }

  prit = health_properties->find("health_collector_sleep");
  if(prit != health_properties->end() && !prit->second.empty()) {
     health_config.health_collector_sleep = atoi(prit->second.c_str()); 
  } else {
     health_config.health_collector_sleep = 60;
  }

  config_data->updateHealthConfig(health_config);
}


/*
 * Try to find the properties of section '[homegear_rpc]' which have been read from configuration file
 * If there is no such section or if a single value is missing the a default
 * value is inserted into the configuration data.
 */
void readHomegearRpcConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections,
                           std::shared_ptr<doorctrl::ConfigData>& config_data) {

  auto it = sections.find("homegear_rpc");
  std::shared_ptr<std::map<std::string, std::string>> homegear_rpc_properties;

   if(it == sections.end()) {
     homegear_rpc_properties = std::make_shared<std::map<std::string, std::string>>();
  } else {
     homegear_rpc_properties = it->second;
  }

  doorctrl::HomegearRpcConfig homegear_rpc_config;

  auto prit = homegear_rpc_properties->find("enabled");
  if(prit != homegear_rpc_properties->end() && !prit->second.empty()) {
     homegear_rpc_config.enabled = (prit->second == "1" || prit->second == "true"); 
  } else {
     homegear_rpc_config.enabled = false; 
  }

  prit = homegear_rpc_properties->find("address");
  if(prit != homegear_rpc_properties->end() && !prit->second.empty()) {
     homegear_rpc_config.address = prit->second.c_str(); 
  } else {
     homegear_rpc_config.address = "127.0.0.1";
  }

  prit = homegear_rpc_properties->find("port");
  if(prit != homegear_rpc_properties->end() && !prit->second.empty()) {
     homegear_rpc_config.port = atoi(prit->second.c_str()); 
  } else {
     homegear_rpc_config.port = 2001;
  }

  prit = homegear_rpc_properties->find("intercom_device_id");
  if(prit != homegear_rpc_properties->end() && !prit->second.empty()) {
     homegear_rpc_config.intercom_device_id = prit->second; 
  } else {
     homegear_rpc_config.intercom_device_id = "Intercom";
  }

  prit = homegear_rpc_properties->find("doorbell_device_id");
  if(prit != homegear_rpc_properties->end() && !prit->second.empty()) {
     homegear_rpc_config.doorbell_device_id = prit->second; 
  } else {
     homegear_rpc_config.doorbell_device_id = "Doorbell";
  }

  prit = homegear_rpc_properties->find("home_button_device_id");
  if(prit != homegear_rpc_properties->end() && !prit->second.empty()) {
     homegear_rpc_config.home_button_device_id = prit->second; 
  } else {
     homegear_rpc_config.home_button_device_id = "Homebutton";
  }

  config_data->updateHomegearRpcConfig(homegear_rpc_config);
}


/*
 * Try to find the properties of section '[ui]' which have been read from configuration file
 * If there is no such section or if a single value is missing the a default
 * value is inserted into the configuration data.
 */
void readUiConfig(const std::map<std::string, std::shared_ptr<std::map<std::string, std::string>>>& sections,
                  std::shared_ptr<doorctrl::ConfigData>& config_data) {

  auto it = sections.find("ui");
  std::shared_ptr<std::map<std::string, std::string>> ui_properties;

  if(it == sections.end()) {
     ui_properties = std::make_shared<std::map<std::string, std::string>>();
  } else {
     ui_properties = it->second;
  }

  doorctrl::UiConfig ui_config;

  auto prit = ui_properties->find("themes_path");
  if(prit != ui_properties->end() && !prit->second.empty()) {
     ui_config.themes_path = prit->second; 
  } else {
     ui_config.themes_path = "/usr/share/doorctrl/styles/doorctrl_dark_theme.css"; 
  }

  prit = ui_properties->find("highlight_color");
  if(prit != ui_properties->end() && !prit->second.empty()) {
     ui_config.highlight_color = prit->second; 
  } else {
     ui_config.highlight_color = "#fca340"; 
  }

  config_data->updateUiConfig(ui_config);
}
