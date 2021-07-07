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

#include "push_button_connector.h"
#include <mutex>
#include <chrono>
#include <functional>
#include <iostream>
#include "common/simple_observer_manager.cpp"
#include "gpio_connector_logger.h"

static std::mutex mutex;

doorctrl::PushButtonConnector::~PushButtonConnector() {

  if(this->gpio_wrapper != nullptr) {
     this->gpio_wrapper->closeDevice(this->config_data->getPushButtonConfig().gpio_input);
  }

}


void doorctrl::PushButtonConnector::setup(std::shared_ptr<ConfigData>& config_data) {
   this->config_data = config_data;
   this->polling_thread = std::make_unique<std::thread>();

   uint8_t gpio_input = this->config_data->getPushButtonConfig().gpio_input; 

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


void doorctrl::PushButtonConnector::start() {
   this->polling_thread = 
     std::make_unique<std::thread>(std::bind(&doorctrl::PushButtonConnector::run_polling_loop, this));
}


bool doorctrl::PushButtonConnector::isPressed() const {
  std::lock_guard<std::mutex> lock(mutex);
  return this->pressed;
}

void doorctrl::PushButtonConnector::addObserver(const std::shared_ptr<PushButtonStateObserver>& observer) {
  this->state_observer_manager.addObserver(observer);
}


void doorctrl::PushButtonConnector::removeObserver(const std::shared_ptr<PushButtonStateObserver>& observer) {
  this->state_observer_manager.removeObserver(observer);
}


void doorctrl::PushButtonConnector::run_polling_loop() {

   /*
    * Pushbutton is low active and a falling edge means button pressed 
    */
   doorctrl::PushButtonConfig push_button_config = this->config_data->getPushButtonConfig();
   this->gpio_wrapper->setEdge(push_button_config.gpio_input, BaseLib::LowLevel::Gpio::GpioEdge::FALLING);

   while(true) {

     int poll_result = this->gpio_wrapper->poll(push_button_config.gpio_input, 1000, push_button_config.debounce);

     if(poll_result >= 0)  {
        this->pressed = !this->pressed;

        if(this->pressed) {
          this->gpio_wrapper->setEdge(push_button_config.gpio_input, BaseLib::LowLevel::Gpio::GpioEdge::RISING);
        } else {
          this->gpio_wrapper->setEdge(push_button_config.gpio_input, BaseLib::LowLevel::Gpio::GpioEdge::FALLING);
        }

        if(push_button_config.inverted) {
           PushButtonStateEvent event(!this->pressed);
           state_observer_manager.fireEvent(event);
        } else {
           PushButtonStateEvent event(this->pressed);
           state_observer_manager.fireEvent(event);
        }
     } else if(poll_result == -1) {

        doorctrl::GpioConnectorLogger::getInstance().getOutput().printWarning("cannot read push button gpio " + std::to_string(push_button_config.gpio_input) + ", error while polling value");

        /*
         * avoid fast running loop in case of gpio problems
         * when the poll method returns immedeately
         */
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
     }
   }
}
