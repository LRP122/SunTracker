#include "PanelFunctions.h"

namespace panel_functions {
    
    void find_optimal_angle_interrupt(void* params){

    std::lock_guard<std::mutex> publish_lock(publish_mtx);
    ESP_LOGI(ourTaskName, "Starting optimal panel routine");

    float maximal_solar_value = 0;
    float current_solar_value = 0;
    int optimal_angle = 0;

    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, minimal_angle);
    vTaskDelay(pdMS_TO_TICKS(2000));
    for(int angle = minimal_angle; angle < maximal_angle;angle++){

        current_solar_value = measure_voltage();
        iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, angle);
        vTaskDelay(pdMS_TO_TICKS(50));

        ESP_LOGI(ourTaskName, "PanelVoltage at %f mV at angle %i", current_solar_value, angle);

        if(current_solar_value > maximal_solar_value){
            maximal_solar_value = current_solar_value;
            optimal_angle = angle;
        }
        }

    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, optimal_angle);
    }

    float measure_voltage(){

        float shunt_voltage, bus_voltage, current; // Voltage of the solar panel
        ina219_get_shunt_voltage(&ina219_dev, &shunt_voltage);
        ina219_get_bus_voltage(&ina219_dev, &bus_voltage);
        ina219_get_current(&ina219_dev, &current);

        printf("VBUS: %.04f V, VSHUNT: %.04f mV, IBUS: %.04f mA\n",
                bus_voltage, shunt_voltage * 1000, current * 1000);

        return bus_voltage;
    }

    void publish_solar_data(float SolarVoltage, float PanelPosition, esp_mqtt_client_handle_t mqtt_handle){
        nlohmann::json_abi_v3_11_3::json solar_data;
        solar_data["PanelVoltage"] = SolarVoltage;
        solar_data["PanelPosition"] = PanelPosition;

        std::string solar_data_str = solar_data.dump();
        const char* solar_data_char = solar_data_str.c_str();
        ESP_LOGI(ourTaskName,"Publishing data to topic /solar/paneldata");
        esp_mqtt_client_publish(mqtt_handle, "/solar/paneldata", solar_data_char , 0, 1, 0);
    }


}