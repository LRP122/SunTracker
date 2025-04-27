#include "PanelFunctions.h"

namespace panel_functions {
    void find_optimal_angle_interrupt(void* params){

    int maximal_solar_value = 0;
    int current_solar_value = 0;
    int optimal_angle = 0;

    for(int angle = minimal_angle; angle < maximal_angle;angle++){
        iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, angle);
        current_solar_value = measure_voltage(solar_panel_voltage);
        float solar_voltage = map(current_solar_value,4096,0,3.3,0);
        ESP_LOGI(ourTaskName,"The voltage at the panel is %f", solar_voltage);
        if(current_solar_value > maximal_solar_value){
            maximal_solar_value = current_solar_value;
            optimal_angle = angle;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
        }
    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, optimal_angle);
    }

    int measure_voltage(adc1_channel_t input_channel){
        int solar_value = 0;
        for (int count = 0; count < sample_cnt ; ++count)
        {
            solar_value += adc1_get_raw(input_channel);
        }
        solar_value /= sample_cnt;
        return solar_value;
    }

}