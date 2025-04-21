#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include <deque>

#include "wifi.h"
#include "mqtt.h"

#include "json.hpp"

WIFI::Wifi wifi;
MQTT::mqtt mqtt;


int led_value = 0;

static void init_hw(void)
{

}

extern "C" void app_main()
{
    wifi.connect_to_wifi();
    esp_mqtt_client_handle_t mqtt_handle = mqtt.mqtt_app_start();
     init_hw();

    while (1)
    {
        // Check Solar

        // Rotate Panel if necessary

    }   
}