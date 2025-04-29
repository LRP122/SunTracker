#include "PanelFunctions.h"

namespace panel_functions {
    
    void find_optimal_angle_interrupt(void* params){

    std::lock_guard<std::mutex> publish_lock(publish_mtx);

    int maximal_solar_value = 0;
    int current_solar_value = 0;
    int optimal_angle = 0;

    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, minimal_angle);
    vTaskDelay(pdMS_TO_TICKS(1000));
    for(int angle = minimal_angle; angle < maximal_angle;angle++){
        iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, angle);
        vTaskDelay(pdMS_TO_TICKS(50));
        current_solar_value = measure_voltage(solar_panel_voltage);
        float solar_voltage = map(current_solar_value,4096,0,3.3,0);
        ESP_LOGI(ourTaskName,"The voltage at the panel is %f", solar_voltage);
        if(current_solar_value > maximal_solar_value){
            maximal_solar_value = current_solar_value;
            optimal_angle = angle;
        }
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

    void publish_solar_data(float SolarVoltage, float PanelPosition,esp_mqtt_client_handle_t mqtt_handle){
        nlohmann::json_abi_v3_11_3::json solar_data;
        solar_data["PanelVoltage"] = SolarVoltage;
        solar_data["PanelPosition"] = PanelPosition;

        std::string solar_data_str = solar_data.dump();
        const char* solar_data_char = solar_data_str.c_str();
        esp_mqtt_client_publish(mqtt_handle, "/solar/paneldata", solar_data_char , 0, 1, 0);
    }


}