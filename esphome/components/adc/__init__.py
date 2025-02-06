from esphome import pins
import esphome.codegen as cg
from esphome.components.esp32 import get_esp32_variant
from esphome.components.esp32.const import (
    VARIANT_ESP32,
    VARIANT_ESP32C2,
    VARIANT_ESP32C3,
    VARIANT_ESP32C6,
    VARIANT_ESP32H2,
    VARIANT_ESP32S2,
    VARIANT_ESP32S3,
)
import esphome.config_validation as cv
from esphome.const import CONF_ANALOG, CONF_INPUT, CONF_NUMBER, PLATFORM_ESP8266
from esphome.core import CORE

CODEOWNERS = ["@esphome/core"]

adc_ns = cg.esphome_ns.namespace("adc")

# ✅ Check ESP-IDF Version
IDF_VERSION_MAJOR = cg.global_ns.ESP_IDF_VERSION_MAJOR if hasattr(cg.global_ns, "ESP_IDF_VERSION_MAJOR") else 4

# 🗂️ ADC Channel Type Based on IDF Version
if IDF_VERSION_MAJOR >= 5:
    adc_channel_t = cg.global_ns.enum("adc_channel_t")
    adc1_channel_t = adc_channel_t  # Alias for backward compatibility
    adc2_channel_t = adc_channel_t
else:
    adc1_channel_t = cg.global_ns.enum("adc1_channel_t")
    adc2_channel_t = cg.global_ns.enum("adc2_channel_t")

# ⚡ ADC Attenuation Modes
ATTENUATION_MODES = {
    "0db": cg.global_ns.ADC_ATTEN_DB_0,
    "2.5db": cg.global_ns.ADC_ATTEN_DB_2_5,
    "6db": cg.global_ns.ADC_ATTEN_DB_6,
    "11db": adc_ns.ADC_ATTEN_DB_12_COMPAT,
    "12db": adc_ns.ADC_ATTEN_DB_12_COMPAT,
    "auto": "auto",
}

# 🔍 ESP32 ADC1 Pin-to-Channel Mappings
ESP32_VARIANT_ADC1_PIN_TO_CHANNEL = {
    VARIANT_ESP32: {
        36: adc1_channel_t.ADC_CHANNEL_0,
        37: adc1_channel_t.ADC_CHANNEL_1,
        38: adc1_channel_t.ADC_CHANNEL_2,
        39: adc1_channel_t.ADC_CHANNEL_3,
        32: adc1_channel_t.ADC_CHANNEL_4,
        33: adc1_channel_t.ADC_CHANNEL_5,
        34: adc1_channel_t.ADC_CHANNEL_6,
        35: adc1_channel_t.ADC_CHANNEL_7,
    },
    VARIANT_ESP32S2: {
        1: adc1_channel_t.ADC_CHANNEL_0,
        2: adc1_channel_t.ADC_CHANNEL_1,
        3: adc1_channel_t.ADC_CHANNEL_2,
        4: adc1_channel_t.ADC_CHANNEL_3,
        5: adc1_channel_t.ADC_CHANNEL_4,
        6: adc1_channel_t.ADC_CHANNEL_5,
        7: adc1_channel_t.ADC_CHANNEL_6,
        8: adc1_channel_t.ADC_CHANNEL_7,
        9: adc1_channel_t.ADC_CHANNEL_8,
        10: adc1_channel_t.ADC_CHANNEL_9,
    },
    VARIANT_ESP32S3: {
        1: adc1_channel_t.ADC_CHANNEL_0,
        2: adc1_channel_t.ADC_CHANNEL_1,
        3: adc1_channel_t.ADC_CHANNEL_2,
        4: adc1_channel_t.ADC_CHANNEL_3,
        5: adc1_channel_t.ADC_CHANNEL_4,
        6: adc1_channel_t.ADC_CHANNEL_5,
        7: adc1_channel_t.ADC_CHANNEL_6,
        8: adc1_channel_t.ADC_CHANNEL_7,
        9: adc1_channel_t.ADC_CHANNEL_8,
        10: adc1_channel_t.ADC_CHANNEL_9,
    },
}

# 🔍 ESP32 ADC2 Pin-to-Channel Mappings
ESP32_VARIANT_ADC2_PIN_TO_CHANNEL = {
    VARIANT_ESP32: {
        4: adc2_channel_t.ADC_CHANNEL_0,
        0: adc2_channel_t.ADC_CHANNEL_1,
        2: adc2_channel_t.ADC_CHANNEL_2,
        15: adc2_channel_t.ADC_CHANNEL_3,
        13: adc2_channel_t.ADC_CHANNEL_4,
        12: adc2_channel_t.ADC_CHANNEL_5,
        14: adc2_channel_t.ADC_CHANNEL_6,
        27: adc2_channel_t.ADC_CHANNEL_7,
        25: adc2_channel_t.ADC_CHANNEL_8,
        26: adc2_channel_t.ADC_CHANNEL_9,
    }
}

# ✅ ADC Pin Validation
def validate_adc_pin(value):
    if str(value).upper() == "VCC":
        if CORE.is_rp2040:
            return pins.internal_gpio_input_pin_schema(29)
        return cv.only_on([PLATFORM_ESP8266])("VCC")

    if str(value).upper() == "TEMPERATURE":
        return cv.only_on_rp2040("TEMPERATURE")

    if CORE.is_esp32:
        conf = pins.internal_gpio_input_pin_schema(value)
        variant = get_esp32_variant()
        pin_number = conf[CONF_NUMBER]

        # Ensure the pin supports ADC
        if (
            pin_number not in ESP32_VARIANT_ADC1_PIN_TO_CHANNEL.get(variant, {})
            and pin_number not in ESP32_VARIANT_ADC2_PIN_TO_CHANNEL.get(variant, {})
        ):
            raise cv.Invalid(f"{variant} doesn't support ADC on this pin")

        return conf

    return pins.gpio_pin_schema({CONF_ANALOG: True, CONF_INPUT: True}, internal=True)(value)
