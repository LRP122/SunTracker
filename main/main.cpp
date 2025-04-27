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

#include "iot_servo.h"

#include "wifi.h"
#include "mqtt.h"

#include "constants.h"
#include "PanelFunctions.h"

WIFI::Wifi wifi;
MQTT::mqtt mqtt;


int led_value = 0;

static void init_hw(void)
{
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
  ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handler, 6 * 10000000));

}

extern "C" void app_main()
{
    init_hw();

    while (1)
    {
        void* nothing = nullptr;
        
    }

}