#include "constants.h"

const int sample_cnt = 32;
const adc1_channel_t solar_panel_voltage = (adc1_channel_t)ADC_CHANNEL_5;
const gpio_num_t servo_pin = (gpio_num_t)2;
const gpio_num_t ina219_i2c_sda = (gpio_num_t) 21;
const gpio_num_t ina219_i2c_scl = (gpio_num_t) 22;
const uint ina219_i2c_addr = 0x40;

int minimal_angle = 0;
int maximal_angle = 180;
std::mutex publish_mtx;
ina219_t ina219_dev;

char* ourTaskName = nullptr; // or initialize properly

void set_task_name(char* taskName) {
    ourTaskName = taskName;
}
