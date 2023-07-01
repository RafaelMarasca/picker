#ifndef _SG90_H_
#define _SG90_H_

#include <inttypes.h>

//50Hz com 16 bits de resolução
#define sg90_CLK_DIV 38.1875f 

typedef struct
{
    uint8_t attached;

    uint8_t gpio_pin;
    uint8_t pwm_channel;
    uint8_t pwm_slice;   
}sg90_t;

sg90_t* sg90_init(uint8_t pin);
void sg90_attach(sg90_t *s, float angle);
void sg90_dettach(sg90_t *s);
uint8_t sg90_isAttached(sg90_t *s);
void sg90_write(sg90_t *s, float angle);


#endif