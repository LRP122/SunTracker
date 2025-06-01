#pragma once

#include "driver/adc.h"  
#include "driver/gpio.h"
#include <ina219.h>
#include <mutex>

extern std::mutex publish_mtx;
extern const int sample_cnt;
extern const adc1_channel_t solar_panel_voltage;
extern const gpio_num_t servo_pin;
extern const gpio_num_t ina219_i2c_sda;
extern const gpio_num_t ina219_i2c_scl;
extern const uint ina219_i2c_addr;
extern int minimal_angle;
extern int maximal_angle;
extern ina219_t ina219_dev;

extern char* ourTaskName;

void set_task_name(char* taskName);

