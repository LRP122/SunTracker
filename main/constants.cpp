#include "constants.h"

const int sample_cnt = 32;
const adc1_channel_t solar_panel_voltage = (adc1_channel_t)ADC_CHANNEL_5;
const gpio_num_t servo_pin = (gpio_num_t)2;

int minimal_angle = 0;
int maximal_angle = 180;

char* ourTaskName = nullptr; // or initialize properly

void set_task_name(char* taskName) {
    ourTaskName = taskName;
}
