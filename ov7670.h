#ifndef _OV7670_H_
#define _OV7670_H_

#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <inttypes.h>

#define HEIGHT 144
#define WIDTH 176
#define HREF 8 
#define VSYNC 9
#define PCLK 10
#define SDA  12
#define SCL 13
#define I2C i2c0
#define XCLK 11


#define OV7670_SLAVE_ADDRESS 0x21 

extern uint8_t ov7670_data[HEIGHT*WIDTH];

void ov7670_init();

int ov7670_write_reg(uint8_t reg, uint8_t data);

int ov7670_read_reg(uint8_t reg, uint8_t* data);

void ov7670_get_frame();

void ov7670_print_data();

#endif //_OV7670_H_