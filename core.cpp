#include "core.h"
#include "pico/multicore.h"
#include "hardware/irq.h"
#include "pico/stdlib.h"
#include <stdio.h>

void (*function)() = NULL;

void core1_entry()
{
    multicore_fifo_push_blocking(CORE_IDLE);


    while(1)
    {

        function = (void(*)()) multicore_fifo_pop_blocking();
        multicore_fifo_push_blocking(CORE_BUSY);

        //if(function)
        (*function)();

        multicore_fifo_push_blocking(CORE_IDLE);
    }
}