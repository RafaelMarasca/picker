#include "display.h"
#include <stdlib.h>

static const uint8_t COLMOD    = 0x3A;
static const uint8_t RAMWR     = 0x2C;
static const uint8_t INV_ON    = 0x21;
static const uint8_t SLEEP_OUT = 0x11;
static const uint8_t NORM_ON   = 0x13;
static const uint8_t DISP_ON   = 0x29;
static const uint8_t PTLAR     = 0x30;
static const uint8_t CASET     = 0x2A;
static const uint8_t RASET     = 0x2B;
static const uint8_t SWRESET   = 0x01;

display_t* init_display(uint8_t dc, uint8_t sck, uint8_t sda, spi_inst_t* spi_port)
{
    display_t* display = (display_t*) malloc(sizeof(display_t));

    if(display != NULL)
    {
        display->dc = dc;
        display->sck = sck;
        display->sda = sda;

        display->spi_port = spi_port;

        spi_init(display->spi_port, 10000000);
        spi_set_format(display->spi_port, 8, SPI_CPOL_1, SPI_CPHA_0, SPI_MSB_FIRST);
        gpio_set_function(display->sda, GPIO_FUNC_SPI);
        gpio_set_function(display->sck, GPIO_FUNC_SPI);
        
        gpio_init(display->dc);
        gpio_set_dir(display->dc, GPIO_OUT);

        display_configure(display);
    } 

    return display;
}

void display_configure(display_t* display)
{
    uint8_t display_mode = 0x55;
    gpio_put(display->dc,0);
    spi_write_blocking(display->spi_port, &SWRESET, 1);
    sleep_ms(10);
    gpio_put(display->dc,0);
    spi_write_blocking(display->spi_port, &COLMOD, 1);
    sleep_ms(10);
    gpio_put(display->dc,1);
    spi_write_blocking(display->spi_port,  &display_mode, 1);
    sleep_ms(10);
    gpio_put(display->dc,0);
    spi_write_blocking(display->spi_port, &SLEEP_OUT, 1);
    sleep_ms(10);
    gpio_put(display->dc,0);
    spi_write_blocking(display->spi_port, &NORM_ON, 1);
    sleep_ms(10);
    gpio_put(display->dc,0);
    spi_write_blocking(display->spi_port, &DISP_ON, 1);
    sleep_ms(10);
    gpio_put(display->dc,0);
    spi_write_blocking(display->spi_port, &INV_ON, 1);
    sleep_ms(10);
}

void display_show_img(display_t* display,const uint8_t* img)
{
    gpio_put(display->dc, 0);
    spi_write_blocking(display->spi_port,&RAMWR, 1);
    sleep_ms(10);
    gpio_put(display->dc, 1);
    spi_write_blocking(display->spi_port,img, 240*240*2);
}

void display_partial(display_t* display, const uint8_t pos[4], const uint8_t* img, uint size)
{
    uint8_t temp[4] = {0, pos[0], 0, pos[1]};
    uint8_t temp2[4] = {0, pos[2], 0, pos[3]};
    gpio_put(display->dc, 0);
    spi_write_blocking(display->spi_port, &RASET, 1);
    gpio_put(display->dc, 1);
    spi_write_blocking(display->spi_port, temp, 4);
    gpio_put(display->dc, 0);
    spi_write_blocking(display->spi_port, &CASET, 1);
    gpio_put(display->dc, 1);
    spi_write_blocking(display->spi_port, temp2, 4);
    gpio_put(display->dc, 0);
    spi_write_blocking(display->spi_port,&RAMWR, 1);
    sleep_ms(10);
    gpio_put(display->dc, 1);
    spi_write_blocking(display->spi_port,img, size);
}

void clear_partial(display_t* display, const uint8_t pos[4], uint size)
{
    uint8_t black = 0x00;
    uint8_t temp[4] = {0, pos[0], 0, pos[1]};
    uint8_t temp2[4] = {0, pos[2], 0, pos[3]};

    gpio_put(display->dc, 0);
    spi_write_blocking(display->spi_port, &RASET, 1);
    gpio_put(display->dc, 1);
    spi_write_blocking(display->spi_port, temp, 4);
    gpio_put(display->dc, 0);
    spi_write_blocking(display->spi_port, &CASET, 1);
    gpio_put(display->dc, 1);
    spi_write_blocking(display->spi_port, temp2, 4);

    gpio_put(display->dc, 0);
    spi_write_blocking(display->spi_port,&RAMWR, 1);
    sleep_ms(10);
    gpio_put(display->dc, 1);

    for(int i = 0; i< size; i++)
        spi_write_blocking(display->spi_port, &black, 1);
}

void display_partial_gray(display_t* display, const uint8_t pos[4], const uint8_t* img, uint size)
{
    uint8_t temp[4] = {0, pos[0], 0, pos[1]};
    uint8_t temp2[4] = {0, pos[2], 0, pos[3]};
    uint8_t pixel[2] = {0,0};
    
    gpio_put(display->dc, 0);
    spi_write_blocking(display->spi_port, &RASET, 1);
    gpio_put(display->dc, 1);
    spi_write_blocking(display->spi_port, temp, 4);
    gpio_put(display->dc, 0);
    spi_write_blocking(display->spi_port, &CASET, 1);
    gpio_put(display->dc, 1);
    spi_write_blocking(display->spi_port, temp2, 4);
    gpio_put(display->dc, 0);
    spi_write_blocking(display->spi_port,&RAMWR, 1);
    sleep_ms(10);
    gpio_put(display->dc, 1);
    for(int i = 0; i < size; i++)
    {
        pixel[0] = (img[i] & 0xF8) | ((img[i] & 0xFC) >> 5);
        pixel[1] = ((img[i] & 0xFC) << 3) | ((img[i] & 0xF8) >> 3);
        spi_write_blocking(display->spi_port, pixel, 2);
    }
}