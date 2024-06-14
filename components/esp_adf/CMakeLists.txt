idf_component_register(SRCS "esp_adf_speaker.cpp" "custom_volume_sensor.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES esphome audio_hal)
