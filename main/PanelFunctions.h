#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h" 
#include "esp_log.h"
#include "iot_servo.h"
#include "constants.h"

namespace panel_functions {
    
    void find_optimal_angle_interrupt(void* params);
    int measure_voltage(adc1_channel_t input_channel);

    template <typename T>
    float map(T input,  float max_input, float min_input, float max_output, float min_output){
        float mapped_value = (1.0 * input/(max_output-min_input)) * max_output + min_input;
        return mapped_value;
    }
}