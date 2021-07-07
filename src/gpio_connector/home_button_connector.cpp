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

#include "home_button_connector.h"
#include <mutex>
#include <chrono>
#include <functional>
#include <iostream>
#include "common/simple_observer_manager.cpp"
#include "gpio_connector_logger.h"

static std::mutex mutex;

doorctrl::HomeButtonConnector::~HomeButtonConnector() {

  if(this->gpio_wrapper != nullptr) {
     this->gpio_wrapper->closeDevice(this->config_data->getPushButtonConfig().gpio_input);
  }

}


void doorctrl::HomeButtonConnector::setup(std::shared_ptr<ConfigData>& config_data) {
   this->config_data = config_data;
   this->polling_thread = std::make_unique<std::thread>();

   uint8_t gpio_input = this->config_data->getHomeButtonConfig().gpio_input;

   std::vector<uint32_t> exported_gpio;
   this->bl = std::make_unique<BaseLib::SharedObjects>();
   this->gpio_wrapper = std::make_unique<BaseLib::LowLevel::Gpio>(this->bl.get(), "/sys/class/gpio/");
   this->gpio_wrapper->exportGpio(gpio_input);
   /*
    *  wait till device permissions have been refreshed by udev rules
    */
   std::this_thread::sleep_for(std::chrono::milliseconds(150));
   this->gpio_wrapper->setDirection(gpio_input, BaseLib::LowLevel::Gpio::GpioDirection::IN);
   this->gpio_wrapper->setEdge(gpio_input, BaseLib::LowLevel::Gpio::GpioEdge::RISING);
   this->gpio_wrapper->openDevice(gpio_input, true);
}


void doorctrl::HomeButtonConnector::start() {
   this->polling_thread = 
     std::make_unique<std::thread>(std::bind(&doorctrl::HomeButtonConnector::run_polling_loop, this));
}


bool doorctrl::HomeButtonConnector::isPressed() const {
  std::lock_guard<std::mutex> lock(mutex);
  return this->pressed;
}

void doorctrl::HomeButtonConnector::addObserver(const std::shared_ptr<HomeButtonStateObserver>& observer) {
  this->state_observer_manager.addObserver(observer);
}


void doorctrl::HomeButtonConnector::removeObserver(const std::shared_ptr<HomeButtonStateObserver>& observer) {
  this->state_observer_manager.removeObserver(observer);
}


void doorctrl::HomeButtonConnector::run_polling_loop() {

   /*
    * Homebutton is low active and a falling edge means button pressed
    */

   uint16_t seconds_pressed = 0;
   doorctrl::HomeButtonConfig home_button_config = this->config_data->getHomeButtonConfig();
   this->gpio_wrapper->setEdge(home_button_config.gpio_input, BaseLib::LowLevel::Gpio::GpioEdge::FALLING);

   while(true) {

     int poll_result = this->gpio_wrapper->poll(home_button_config.gpio_input, 1000, home_button_config.debounce);

     if(poll_result >= 0)  { 
        this->pressed = !this->pressed; 

        if(this->pressed) {
          this->gpio_wrapper->setEdge(home_button_config.gpio_input, BaseLib::LowLevel::Gpio::GpioEdge::RISING);
        } else {
          seconds_pressed = 0; 
          this->gpio_wrapper->setEdge(home_button_config.gpio_input, BaseLib::LowLevel::Gpio::GpioEdge::FALLING);
        }
         
        if(home_button_config.inverted) {
           HomeButtonStateEvent event(!this->pressed);
           state_observer_manager.fireEvent(event);
        } else {
           HomeButtonStateEvent event(this->pressed);
           state_observer_manager.fireEvent(event);
        }
     } else if(poll_result == -1) {
        doorctrl::GpioConnectorLogger::getInstance().getOutput().printWarning("cannot read home button gpio " + std::to_string(home_button_config.gpio_input) + ", error while polling value");
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
     } else if(poll_result == -2) {

        if(this->pressed) {
 
           doorctrl::GpioConnectorLogger::getInstance().getOutput().printDebug("home button pressed for " + std::to_string(seconds_pressed) + " seconds");
           if(seconds_pressed >= 20) {
              doorctrl::GpioConnectorLogger::getInstance().getOutput().printDebug("firing home button state event");
              HomeButtonStateEvent event(this->pressed, true);
              state_observer_manager.fireEvent(event);
              seconds_pressed = 0;
              this->pressed = false;
              this->gpio_wrapper->setEdge(home_button_config.gpio_input, BaseLib::LowLevel::Gpio::GpioEdge::FALLING);
           } else {
              seconds_pressed++;
           }
        }
     }
   }
}
