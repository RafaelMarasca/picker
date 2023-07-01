#include "ov7670.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

uint8_t ov7670_data[HEIGHT*WIDTH] = {};

void ov7670_init()
{
    
    for(int i = 0; i < 8; i++)
    {
        gpio_init(i);
        gpio_set_dir(i, GPIO_IN);
    } 

    gpio_init(HREF);
    gpio_set_dir(HREF, GPIO_IN);
    gpio_init(VSYNC);
    gpio_set_dir(VSYNC, GPIO_IN);
    gpio_init(PCLK);
    gpio_set_dir(PCLK, GPIO_IN);


    i2c_init(I2C, 400000);
    gpio_set_function(SDA, GPIO_FUNC_I2C);
    gpio_set_function(SCL, GPIO_FUNC_I2C);
    gpio_pull_up(SDA);
    gpio_pull_up(SCL);

    gpio_set_function(XCLK, GPIO_FUNC_PWM);
    pwm_set_clkdiv(pwm_gpio_to_slice_num(XCLK), 1.3021);
    pwm_set_wrap(pwm_gpio_to_slice_num(XCLK), 3);
    pwm_set_gpio_level(XCLK, 2);
    pwm_set_enabled(pwm_gpio_to_slice_num(XCLK), true);

    uint8_t value = 0x00;
   	uint hstart		= 456; /* Empirically determined */
	uint hstop		=  24;
	uint vstart		=  14;
	uint vstop		= 494;

    ov7670_write_reg( 0x12, 0x80); 
    ov7670_write_reg( 0x3A, 0x0D);
    ov7670_write_reg( 0x0C, 0x0C);

    /* Configuration for PCLK 12MHz and 24MHz input (Tweaked Empirically) (30fps)*/

    ov7670_write_reg( 0x70, 0x35);
    ov7670_write_reg( 0x71, 0x35);
    ov7670_write_reg( 0x3E, 0x11);
    ov7670_write_reg( 0x72, 0x11);
    ov7670_write_reg( 0x73, 0xf1);
    ov7670_write_reg( 0xa2, 0x52);

    ov7670_write_reg( 0x11, 0x01);
    ov7670_write_reg( 0x6b, 0x0a);
    ov7670_write_reg( 0x2a, 0x00);
    ov7670_write_reg( 0x2b, 0x00);
    ov7670_write_reg( 0x92, 0x66);
    ov7670_write_reg( 0x93, 0x00);
    ov7670_write_reg( 0x3b, 0x0a);

    ov7670_write_reg( 0x17, (hstart >> 3) & 0xff);
    ov7670_write_reg( 0x18, (hstop >> 3) & 0xff);
    value = ov7670_read_reg( 0x32, &value);
    value = (value & 0xC0) | ((hstop & 0x7) << 3) | (hstart & 0x07);
    ov7670_write_reg( 0x32, value);
    //ov7670_write_reg( 0x11, 0x01);

    /* VSTART and VSTOP configuration */
    ov7670_write_reg( 0x19, (vstart >> 2) & 0xff);
    ov7670_write_reg( 0x1a, (vstop >> 2) & 0xff);
    value = ov7670_read_reg( 0x03, &value);
    value = (value & 0xF0) | ((vstop & 0x03) << 2) | (vstart & 0x03);
    ov7670_write_reg( 0x03, value);

    //ov7670_write_reg( 0x11, 0x01);

    /* Color Correction Matrix Parameters */
    ov7670_write_reg( 0x40, 0xc0);
    ov7670_write_reg( 0x14, 0x48); 
    ov7670_write_reg( 0x4f, 0x80);		
    ov7670_write_reg( 0x50, 0x80);		
    ov7670_write_reg( 0x51, 0x00);		
    ov7670_write_reg( 0x52, 0x22);		
    ov7670_write_reg( 0x53, 0x5e);		
    ov7670_write_reg( 0x54, 0x80);		
    ov7670_write_reg( 0x3d, 0x80 | 0x40);
    ov7670_write_reg( 0xff, 0xff);

    /* Gamma Curves Parameters */
    ov7670_write_reg(0x7a, 0x20);		ov7670_write_reg(0x7b, 0x10);
    ov7670_write_reg(0x7c, 0x1e);		ov7670_write_reg(0x7d, 0x35);
    ov7670_write_reg(0x7e, 0x5a);		ov7670_write_reg(0x7f, 0x69);
    ov7670_write_reg(0x80, 0x76);		ov7670_write_reg(0x81, 0x80);
    ov7670_write_reg(0x82, 0x88);		ov7670_write_reg(0x83, 0x8f);
    ov7670_write_reg(0x84, 0x96);		ov7670_write_reg(0x85, 0xa3);
    ov7670_write_reg(0x86, 0xaf);		ov7670_write_reg(0x87, 0xc4);
    ov7670_write_reg(0x88, 0xd7);		ov7670_write_reg(0x89, 0xe8);

    /* Automatic Gain Configuration*/
    ov7670_write_reg( 0x13, 0x80 | 0x40 | 0x20);
    ov7670_write_reg( 0x00, 0);	
    ov7670_write_reg( 0x10, 0);
    ov7670_write_reg( 0x0d, 0x40); 
    ov7670_write_reg( 0x14, 0x18); 
    ov7670_write_reg( 0xa5, 0x05);	
    ov7670_write_reg( 0xab, 0x07);
    ov7670_write_reg( 0x24, 0x95);	
    ov7670_write_reg( 0x25, 0x33);
    ov7670_write_reg( 0x26, 0xe3);	
    ov7670_write_reg( 0x9f, 0x78);
    ov7670_write_reg( 0xa0, 0x68);	
    ov7670_write_reg( 0xa1, 0x03); 
    ov7670_write_reg( 0xa6, 0xd8);	
    ov7670_write_reg( 0xa7, 0xd8);
    ov7670_write_reg( 0xa8, 0xf0);	
    ov7670_write_reg( 0xa9, 0x90);
    ov7670_write_reg( 0xaa, 0x94);
    ov7670_write_reg( 0x13, 0x80 | 0x40 | 0x20 | 0x04 | 0x01);

    ov7670_write_reg( 0x0e, 0x61);	    ov7670_write_reg( 0x0f, 0x4b);
    ov7670_write_reg( 0x16, 0x02);		ov7670_write_reg( 0x1e, 0x07);
    ov7670_write_reg( 0x21, 0x02);		ov7670_write_reg( 0x22, 0x91);
    ov7670_write_reg( 0x29, 0x07);		ov7670_write_reg( 0x33, 0x0b);
    ov7670_write_reg( 0x35, 0x0b);		ov7670_write_reg( 0x37, 0x1d);
    ov7670_write_reg( 0x38, 0x71);		ov7670_write_reg( 0x39, 0x2a);
    ov7670_write_reg( 0x3c, 0x78);	    ov7670_write_reg( 0x4d, 0x40);
    ov7670_write_reg( 0x4e, 0x20);		ov7670_write_reg( 0x69, 0x00);
    ov7670_write_reg( 0x74, 0x10);       ov7670_write_reg( 0x8d, 0x4f);
    ov7670_write_reg( 0x8e, 0x00);       ov7670_write_reg( 0x8f, 0x00);
    ov7670_write_reg( 0x90, 0x00);       ov7670_write_reg( 0x91, 0x00);
    ov7670_write_reg( 0x96, 0x00);       ov7670_write_reg( 0x9a, 0x00);
    ov7670_write_reg( 0xb0, 0x84);       ov7670_write_reg( 0xb1, 0x0c);
    ov7670_write_reg( 0xb2, 0x0e);       ov7670_write_reg( 0xb3, 0x82);
    ov7670_write_reg( 0xb8, 0x0a);

    ov7670_write_reg( 0x43, 0x0a);		ov7670_write_reg( 0x44, 0xf0);
    ov7670_write_reg( 0x45, 0x34);		ov7670_write_reg( 0x46, 0x58);
    ov7670_write_reg( 0x47, 0x28);		ov7670_write_reg( 0x48, 0x3a);
    ov7670_write_reg( 0x59, 0x88);		ov7670_write_reg( 0x5a, 0x88);
    ov7670_write_reg( 0x5b, 0x44);		ov7670_write_reg( 0x5c, 0x67);
    ov7670_write_reg( 0x5d, 0x49);		ov7670_write_reg( 0x5e, 0x0e);
    ov7670_write_reg( 0x6c, 0x0a);		ov7670_write_reg( 0x6d, 0x55);
    ov7670_write_reg( 0x6e, 0x11);		ov7670_write_reg( 0x6f, 0x9f); 
    ov7670_write_reg( 0x6a, 0x40);		ov7670_write_reg( 0x01, 0x40);
    ov7670_write_reg( 0x02, 0x60);
    ov7670_write_reg( 0x13, 0x80 | 0x40 | 0x20 | 0x04 | 0x01 | 0x02);

    ov7670_write_reg( 0x4f, 0x80);		
    ov7670_write_reg( 0x50, 0x80);
    ov7670_write_reg( 0x51, 0x00);	        
    ov7670_write_reg( 0x52, 0x22);
    ov7670_write_reg( 0x53, 0x5e);		
    ov7670_write_reg( 0x54, 0x80);
    ov7670_write_reg( 0x58, 0x9e);

    ov7670_write_reg( 0x41, 0x08);	     ov7670_write_reg( 0x3f, 0);
    ov7670_write_reg( 0x75, 0x05);	     ov7670_write_reg( 0x76, 0xe1);
    ov7670_write_reg( 0x4c, 0x00);		 ov7670_write_reg( 0x77, 0x01);
    ov7670_write_reg( 0x3d, 0xc3);	     ov7670_write_reg( 0x4b, 0x09);
    ov7670_write_reg( 0xc9, 0x60);	     ov7670_write_reg( 0x41, 0x38);
    ov7670_write_reg( 0x56, 0x40);

    ov7670_write_reg( 0x34, 0x11 );		 ov7670_write_reg( 0x3b, 0x02 | 0x10);
    ov7670_write_reg( 0xa4, 0x88 );		 ov7670_write_reg( 0x96, 0x00);
    ov7670_write_reg( 0x97, 0x30 );		 ov7670_write_reg( 0x98, 0x20);
    ov7670_write_reg( 0x99, 0x30 );		 ov7670_write_reg( 0x9a, 0x84);
    ov7670_write_reg( 0x9b, 0x29 );		 ov7670_write_reg( 0x9c, 0x03);
    ov7670_write_reg( 0x9d, 0x4c );		 ov7670_write_reg( 0x9e, 0x3f);
    ov7670_write_reg( 0x78, 0x04 );

    ov7670_write_reg(0x79, 0x01);		ov7670_write_reg(0xc8, 0xf0);
	ov7670_write_reg(0x79, 0x0f);		ov7670_write_reg(0xc8, 0x00);
	ov7670_write_reg(0x79, 0x10);		ov7670_write_reg(0xc8, 0x7e);
	ov7670_write_reg(0x79, 0x0a);		ov7670_write_reg(0xc8, 0x80);
	ov7670_write_reg(0x79, 0x0b);		ov7670_write_reg(0xc8, 0x01);
	ov7670_write_reg(0x79, 0x0c);		ov7670_write_reg(0xc8, 0x0f);
	ov7670_write_reg(0x79, 0x0d);		ov7670_write_reg(0xc8, 0x20);
	ov7670_write_reg(0x79, 0x09);		ov7670_write_reg(0xc8, 0x80);
	ov7670_write_reg(0x79, 0x02);		ov7670_write_reg(0xc8, 0xc0);
	ov7670_write_reg(0x79, 0x03);		ov7670_write_reg(0xc8, 0x40);
	ov7670_write_reg(0x79, 0x05);		ov7670_write_reg(0xc8, 0x30);
	ov7670_write_reg(0x79, 0x26);
}


int ov7670_write_reg(uint8_t reg, uint8_t data)
{
    uint8_t buffer[] = {reg, data};
    return i2c_write_blocking(I2C, OV7670_SLAVE_ADDRESS, buffer, 2, false);
}

int ov7670_read_reg( uint8_t reg, uint8_t* data)
{
    i2c_write_blocking(I2C, OV7670_SLAVE_ADDRESS, &reg, 1, false);
    return i2c_read_blocking(I2C, OV7670_SLAVE_ADDRESS, data, 1, false);
}

void ov7670_get_frame()
{
    int ind = 0;
    while(!gpio_get(VSYNC));
    while(gpio_get(VSYNC));

    for(int i = 0; i<HEIGHT; i++)
    {
        while(!gpio_get(HREF));

        for(int j = 0; j < WIDTH; j++)
        {
            while(!gpio_get(PCLK));
            while(gpio_get(PCLK));
            
            while(!gpio_get(PCLK));
            ov7670_data[ind++] = (uint8_t) ((gpio_get_all()) & 0xFF);
            while(gpio_get(PCLK));
        }
        while(gpio_get(HREF));
    }
}

void ov7670_print_data()
{
    for(int i = 0; i < (HEIGHT*WIDTH); i++)
    {
        if(ov7670_data[i] == '\n')
            printf("%c",ov7670_data[i] + 1);
        else
            printf("%c", ov7670_data[i]);
    }
}