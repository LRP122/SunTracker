#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include <ina219.h>
#include "esp_timer.h"
#include <deque>
#include <string>
#include <mutex>
#include "json.hpp"

#include "iot_servo.h"

#include "wifi.h"

#include "constants.h"
#include "PanelFunctions.h"

using json = nlohmann::json;

const i2c_port_t i2c_port = (i2c_port_t) 0;

WIFI::Wifi wifi;
MQTT::mqtt mqtt;

float bus_voltage, shunt_voltage, current, power;

static void init_hw(void)
{
    wifi.connect_to_wifi();

    set_task_name(pcTaskGetName(NULL));

    i2cdev_init();
    ina219_init_desc(&ina219_dev, ina219_i2c_addr, i2c_port, ina219_i2c_sda, ina219_i2c_scl);
    ina219_init(&ina219_dev);
    ina219_configure(&ina219_dev, INA219_BUS_RANGE_16V, INA219_GAIN_1,
            INA219_RES_12BIT_128S, INA219_RES_12BIT_128S, INA219_MODE_CONT_SHUNT_BUS);
    ina219_calibrate(&ina219_dev, (float)100 / 1000.0f);

    adc1_config_width(ADC_WIDTH_BIT_10);
    adc1_config_channel_atten(solar_panel_voltage, ADC_ATTEN_DB_11);
     servo_config_t servo_cfg = {
    .max_angle = 180,
    .min_width_us = 500,
    .max_width_us = 2500,
    .freq = 50,
    .timer_number = LEDC_TIMER_0,
    .channels = {
            .servo_pin = {
                servo_pin
            }
        },
    .channel_number = 1,
    };
    iot_servo_init(LEDC_LOW_SPEED_MODE, &servo_cfg);

    const esp_timer_create_args_t my_timer_args = {
      .callback = &panel_functions::find_optimal_angle_interrupt,
      .name = "AdjustingSolarPanelTimer"};
    esp_timer_handle_t timer_handler;
    ESP_ERROR_CHECK(esp_timer_create(&my_timer_args, &timer_handler));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handler,  6*5*10000000));

    void* params = nullptr;

    panel_functions::find_optimal_angle_interrupt(params);

}

extern "C" void app_main()
{   
    init_hw();
    esp_mqtt_client_handle_t mqtt_handle = mqtt.mqtt_app_start();
    float current;

    while(1){

        vTaskDelay(pdMS_TO_TICKS(5000));
        float panel_voltage = panel_functions::measure_voltage();
        float angle;

        {
            std::lock_guard<std::mutex> publish_lock(publish_mtx);
            iot_servo_read_angle(LEDC_LOW_SPEED_MODE, 0, &angle);
            panel_functions::publish_solar_data(panel_voltage,angle,mqtt_handle);
        }

    }
}