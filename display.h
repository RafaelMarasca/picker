#ifndef DISPLAY_H
#define DISPLAY_H

#include <pico/stdlib.h>
#include <hardware/spi.h>

typedef struct 
{
    uint8_t dc;
    uint8_t sck;
    uint8_t sda;
    spi_inst_t* spi_port;
} display_t;


display_t* init_display(uint8_t dc, uint8_t sck, uint8_t sda, spi_inst_t* spi_port);
void display_configure(display_t* display);
void display_show_img(display_t* display, const uint8_t* img);
void display_partial(display_t* display, const uint8_t pos[4], const uint8_t* img, uint size);
void clear_partial(display_t* display, const uint8_t pos[4], uint size);
void display_partial_gray(display_t* display, const uint8_t pos[4], const uint8_t* img, uint size);


#endif //DISPLAY