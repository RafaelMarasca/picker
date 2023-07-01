#ifndef INPUT_H
#define INPUT_H

#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "pico/sync.h"

#define UP     0b10000000
#define DOWN   0b01000000
#define LEFT   0b00100000
#define RIGHT  0b00010000
#define ENTER  0b00001000
#define CANCEL 0b00000100
#define ZUP    0b00000010
#define ZDOWN  0b00000001


extern uint32_t buffer[3];

extern mutex_t adc_buffer_mutex;

bool adc_task(repeating_timer_t *rt);

void init_input();
uint8_t get_button(uint32_t reading);
uint8_t handle_input();

#endif //INPUT_H