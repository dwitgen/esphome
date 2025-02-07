#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/voltage_sampler/voltage_sampler.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"

#ifdef USE_ESP32
  #include "esp_adc/adc_cali.h"
  #include "esp_adc/adc_cali_scheme.h"
  #include "esp_adc/adc_oneshot.h"
#endif  // USE_ESP32

namespace esphome {
namespace adc {

#ifdef USE_ESP32
// Compatibility for attenuation settings
#if (ESP_IDF_VERSION_MAJOR >= 5)
  static const adc_atten_t ADC_ATTEN_DB_12_COMPAT = ADC_ATTEN_DB_12;
#else
  static const adc_atten_t ADC_ATTEN_DB_12_COMPAT = ADC_ATTEN_DB_11;
#endif
#endif  // USE_ESP32

class ADCSensor : public sensor::Sensor, public PollingComponent, public voltage_sampler::VoltageSampler {
 public:
#ifdef USE_ESP32
  // Set the attenuation for this pin
  void set_attenuation(adc_atten_t attenuation) { this->attenuation_ = attenuation; }

  // Configure ADC channel
  void set_channel(adc_channel_t channel) {
    this->channel_ = channel;
  }

  // Enable auto-range mode
  void set_autorange(bool autorange) { this->autorange_ = autorange; }
#endif  // USE_ESP32

  // Core ESPHome lifecycle functions
  void update() override;
  void setup() override;
  void dump_config() override;

  // `HARDWARE_LATE` setup priority
  float get_setup_priority() const override;

  // Additional Configuration
  void set_pin(InternalGPIOPin *pin) { this->pin_ = pin; }
  void set_output_raw(bool output_raw) { this->output_raw_ = output_raw; }
  void set_sample_count(uint8_t sample_count);
  float sample() override;

#ifdef USE_ESP8266
  std::string unique_id() override;
#endif  // USE_ESP8266

#ifdef USE_RP2040
  void set_is_temperature() { this->is_temperature_ = true; }
#endif  // USE_RP2040

 protected:
  InternalGPIOPin *pin_;
  bool output_raw_{false};
  uint8_t sample_count_{1};

#ifdef USE_RP2040
  bool is_temperature_{false};
#endif  // USE_RP2040

#ifdef USE_ESP32
  adc_atten_t attenuation_{ADC_ATTEN_DB_0};
  adc_channel_t channel_{ADC_CHANNEL_0};  // Simplified for ESP-IDF v5
  bool autorange_{false};

  adc_oneshot_unit_handle_t adc_handle_{nullptr};  // Handle for ADC one-shot driver
  adc_cali_handle_t cal_handle_{nullptr};          // Handle for calibration
#endif  // USE_ESP32
};

}  // namespace adc
}  // namespace esphome
