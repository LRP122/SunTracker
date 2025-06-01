#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h" 
#include "esp_log.h"
#include "iot_servo.h"
#include "constants.h"
#include <ina219.h>
#include "json.hpp"
#include "mqtt.h"

namespace panel_functions {
    
    void find_optimal_angle_interrupt(void* params);
    float measure_voltage();

    template <typename T>
    float map(T input,  float max_input, float min_input, float max_output, float min_output){
        float mapped_value = (((1.0 * input/(1.0* max_input-1.0*min_input)) * max_output) + min_output);
        return mapped_value;
    }

    void publish_solar_data(float SolarVoltage, float PanelPosition,esp_mqtt_client_handle_t mqtt_handle);
}