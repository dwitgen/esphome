#ifdef USE_ESP32

#include "adc_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace adc {

static const char *const TAG = "adc.esp32";

#if ESP_IDF_VERSION_MAJOR >= 5
  #include "esp_adc/adc_cali.h"
  #include "esp_adc/adc_oneshot.h"
#endif

void ADCSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ADC '%s'...", this->get_name().c_str());

#if ESP_IDF_VERSION_MAJOR >= 5
  adc_oneshot_unit_init_cfg_t init_cfg = {
    .unit_id = ADC_UNIT_1
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_cfg, &adc_handle_));

  adc_oneshot_chan_cfg_t channel_cfg = {
    .atten = this->attenuation_,
    .bitwidth = ADC_BITWIDTH_DEFAULT
  };
  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle_, this->channel1_, &channel_cfg));

  adc_cali_curve_fitting_config_t cal_cfg = {
    .unit_id = ADC_UNIT_1,
    .atten = this->attenuation_,
    .bitwidth = ADC_BITWIDTH_DEFAULT
  };
  ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cal_cfg, &cal_handle_));

#else
  if (this->channel1_ != ADC1_CHANNEL_MAX) {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(this->channel1_, this->attenuation_);
  } else if (this->channel2_ != ADC2_CHANNEL_MAX) {
    adc2_config_channel_atten(this->channel2_, this->attenuation_);
  }

  esp_adc_cal_characterize(ADC_UNIT_1, this->attenuation_, ADC_WIDTH_BIT_12, 1100, &cal_characteristics_[this->attenuation_]);
#endif
}

void ADCSensor::dump_config() {
  LOG_SENSOR("", "ADC Sensor", this);
  LOG_PIN("  Pin: ", this->pin_);
  ESP_LOGCONFIG(TAG, "  Attenuation: %d", this->attenuation_);
  ESP_LOGCONFIG(TAG, "  Samples: %d", this->sample_count_);
  LOG_UPDATE_INTERVAL(this);
}

float ADCSensor::sample() {
#if ESP_IDF_VERSION_MAJOR >= 5
  int raw = 0;
  ESP_ERROR_CHECK(adc_oneshot_read(adc_handle_, this->channel1_, &raw));

  int voltage = 0;
  ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cal_handle_, raw, &voltage));

  return this->output_raw_ ? raw : voltage / 1000.0f;

#else
  int raw = 0;
  if (this->channel1_ != ADC1_CHANNEL_MAX) {
    raw = adc1_get_raw(this->channel1_);
  } else if (this->channel2_ != ADC2_CHANNEL_MAX) {
    adc2_get_raw(this->channel2_, ADC_WIDTH_BIT_12, &raw);
  }

  if (raw == -1) {
    return NAN;
  }

  uint32_t voltage = esp_adc_cal_raw_to_voltage(raw, &cal_characteristics_[this->attenuation_]);
  return this->output_raw_ ? raw : voltage / 1000.0f;
#endif
}

}  // namespace adc
}  // namespace esphome

#endif  // USE_ESP32
