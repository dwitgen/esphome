#pragma once

#include "esphome.h"

namespace esphome {
namespace custom_components {

class CustomVolumeSensor : public sensor::Sensor, public Component {
 public:
  void setup() override {
    ESP_LOGCONFIG(TAG, "Setting up Custom Volume Sensor...");
  }

  void update_volume(int volume) {
    ESP_LOGI(TAG, "Updating volume to %d", volume);
    this->publish_state(volume);
  }
};

}  // namespace custom_components
}  // namespace esphome
