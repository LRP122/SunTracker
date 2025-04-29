#pragma once

#include "driver/adc.h"  
#include "driver/gpio.h" 
#include <mutex>

extern std::mutex publish_mtx;
extern const int sample_cnt;
extern const adc1_channel_t solar_panel_voltage;
extern const gpio_num_t servo_pin;
extern int minimal_angle;
extern int maximal_angle;

extern char* ourTaskName;

void set_task_name(char* taskName);

