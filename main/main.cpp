#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/ledc.h"
#include "esp_log.h"
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

WIFI::Wifi wifi;
MQTT::mqtt mqtt;

static void init_hw(void)
{
    wifi.connect_to_wifi();

    set_task_name(pcTaskGetName(NULL));


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
    while(1){
        vTaskDelay(pdMS_TO_TICKS(5000));
        float raw_voltage = panel_functions::measure_voltage(solar_panel_voltage);
        float panel_voltage = panel_functions::map(raw_voltage,4096,0,3.3,0) * 2; // Voltage divider needs the *2
        float angle = 0;
        iot_servo_read_angle(LEDC_LOW_SPEED_MODE, 0, &angle);
        {
        std::lock_guard<std::mutex> publish_lock(publish_mtx);
        panel_functions::publish_solar_data(panel_voltage,angle,mqtt_handle);
        }

    }
}