#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace custom_components {

class CustomVolumeSensor : public sensor::Sensor, public Component {
 public:
  void setup() override {
    ESP_LOGCONFIG("custom_volume_sensor", "Setting up Custom Volume Sensor...");
  }

  void update_volume(int volume) {
    ESP_LOGI("custom_volume_sensor", "Updating volume to %d", volume);
    this->publish_state(volume);
  }
};

}  // namespace custom_components
}  // namespace esphome
