#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/voltage_sampler/voltage_sampler.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"

#ifdef USE_ESP32
  #if ESP_IDF_VERSION_MAJOR >= 5
    #include "esp_adc/adc_cali.h"
    #include "esp_adc/adc_cali_scheme.h"
    #include "esp_adc/adc_oneshot.h"
  #else
    #include <esp_adc_cal.h>
    #include "driver/adc.h"
  #endif
#endif  // USE_ESP32

namespace esphome {
namespace adc {

#ifdef USE_ESP32
// Compatibility for attenuation settings
#if (ESP_IDF_VERSION_MAJOR == 4 && ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 7)) || \
    (ESP_IDF_VERSION_MAJOR >= 5)
static const adc_atten_t ADC_ATTEN_DB_12_COMPAT = ADC_ATTEN_DB_12;
#else
static const adc_atten_t ADC_ATTEN_DB_12_COMPAT = ADC_ATTEN_DB_11;
#endif
#endif  // USE_ESP32

class ADCSensor : public sensor::Sensor, public PollingComponent, public voltage_sampler::VoltageSampler {
 public:
#ifdef USE_ESP32
  /// Set attenuation
  void set_attenuation(adc_atten_t attenuation) { this->attenuation_ = attenuation; }

  #if ESP_IDF_VERSION_MAJOR >= 5
    /// For ESP-IDF v5: Single channel setup
    void set_channel(adc_channel_t channel) { this->channel_ = channel; }
  #else
    /// For older ESP-IDF versions: Separate ADC1 and ADC2 channels
    void set_channel1(adc1_channel_t channel) {
      this->channel1_ = channel;
      this->channel2_ = ADC2_CHANNEL_MAX;
    }
    void set_channel2(adc2_channel_t channel) {
      this->channel2_ = channel;
      this->channel1_ = ADC1_CHANNEL_MAX;
    }
  #endif

  /// Enable auto-range mode
  void set_autorange(bool autorange) { this->autorange_ = autorange; }
#endif  // USE_ESP32

  /// Core ESPHome lifecycle functions
  void update() override;
  void setup() override;
  void dump_config() override;

  /// `HARDWARE_LATE` setup priority
  float get_setup_priority() const override;

  /// Additional Configuration
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
  bool autorange_{false};

  #if ESP_IDF_VERSION_MAJOR >= 5
    adc_oneshot_unit_handle_t adc_handle_;
    adc_cali_handle_t cal_handle_;
    adc_channel_t channel_;  // Single channel for IDF v5
  #else
    adc1_channel_t channel1_{ADC1_CHANNEL_MAX};
    adc2_channel_t channel2_{ADC2_CHANNEL_MAX};
    esp_adc_cal_characteristics_t cal_characteristics_[ADC_ATTEN_MAX] = {};
  #endif
#endif  // USE_ESP32
};

}  // namespace adc
}  // namespace esphome
