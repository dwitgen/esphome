#ifdef USE_ESP32

#include "adc_sensor.h"
#include "esphome/core/log.h"

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_oneshot.h"

namespace esphome {
namespace adc {

static const char *const TAG = "adc.esp32";

void ADCSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ADC '%s'...", this->get_name().c_str());

  // Initialize ADC One-shot mode
  adc_oneshot_unit_init_cfg_t init_cfg = {
      .unit_id = ADC_UNIT_1,  // Using ADC Unit 1
      .ulp_mode = ADC_ULP_MODE_DISABLE
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_cfg, &adc_handle_));

  // Configure ADC Channel
  adc_oneshot_chan_cfg_t channel_cfg = {
      .atten = this->attenuation_,
      .bitwidth = ADC_BITWIDTH_DEFAULT
  };
  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle_, this->channel_, &channel_cfg));

  // Setup Calibration
  adc_cali_curve_fitting_config_t cal_cfg = {
      .unit_id = ADC_UNIT_1,
      .atten = this->attenuation_,
      .bitwidth = ADC_BITWIDTH_DEFAULT
  };
  ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cal_cfg, &cal_handle_));
}

void ADCSensor::dump_config() {
  LOG_SENSOR("", "ADC Sensor", this);
  LOG_PIN("  Pin: ", this->pin_);
  
  if (this->autorange_) {
    ESP_LOGCONFIG(TAG, "  Attenuation: auto");
  } else {
    switch (this->attenuation_) {
      case ADC_ATTEN_DB_0:
        ESP_LOGCONFIG(TAG, "  Attenuation: 0dB");
        break;
      case ADC_ATTEN_DB_2_5:
        ESP_LOGCONFIG(TAG, "  Attenuation: 2.5dB");
        break;
      case ADC_ATTEN_DB_6:
        ESP_LOGCONFIG(TAG, "  Attenuation: 6dB");
        break;
      case ADC_ATTEN_DB_12:
        ESP_LOGCONFIG(TAG, "  Attenuation: 12dB");
        break;
      default:
        ESP_LOGCONFIG(TAG, "  Attenuation: Unknown");
        break;
    }
  }

  ESP_LOGCONFIG(TAG, "  Samples: %d", this->sample_count_);
  LOG_UPDATE_INTERVAL(this);
}

float ADCSensor::sample() {
  int raw = 0;
  int voltage = 0;
  uint32_t sum_raw = 0;

  // Multisampling support
  for (uint8_t i = 0; i < this->sample_count_; i++) {
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle_, this->channel_, &raw));
    sum_raw += raw;
  }

  int average_raw = sum_raw / this->sample_count_;

  if (this->output_raw_) {
    return average_raw;  // Return raw ADC value
  }

  // Apply Calibration
  ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cal_handle_, average_raw, &voltage));

  return voltage / 1000.0f;  // Convert mV to V
}

}  // namespace adc
}  // namespace esphome

#endif  // USE_ESP32
