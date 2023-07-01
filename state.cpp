#include <stdlib.h>
#include "pico/multicore.h"
#include "gui.h"
#include "display.h"
#include "state.h"
#include "pico/stdlib.h"
#include "core.h"
#include "hardware/irq.h"
#include <stdio.h>


static state_t* current_state = NULL;
uint32_t state_flag = CORE_IDLE;


void core0_interrupt()
{
    state_flag = multicore_fifo_pop_blocking();

    if(state_flag == CORE_IDLE)
        set_state(current_state->exit);

    irq_set_enabled(SIO_IRQ_PROC0, false);
    multicore_fifo_clear_irq();
}

state_t* new_state(gui_t* gui, void (*func)())
{
    state_t* state = (state_t*)malloc(sizeof(state_t));
    state->func = func;
    state->prev = NULL;
    state->next = NULL;
    state->enter = NULL;
    state->exit = NULL;
    state->gui = gui;

    return state;
}

void init_state()
{   
    display_t* display = init_display(20, 14, 15, spi1);
    
    if(display)
        init_gui(display);
}

void run_state()
{
    show_gui(current_state->gui);
}

void set_state(state_t* state)
{
    if(current_state)
        hide_gui(current_state->gui);
    
    current_state = state;
}

void set_next_state()
{
    if(current_state->next)
        set_state(current_state->next);
}

void set_prev_state()
{
    if(current_state->prev)
        set_state(current_state->prev);
}

void state_act()
{
    if(current_state->func && state_flag == CORE_IDLE)
    {
        
        multicore_fifo_push_blocking((uintptr_t)(current_state->func));
        state_flag = multicore_fifo_pop_blocking();
        
        if(current_state->enter)
        {
            state_t* temp = current_state;
            set_state(current_state->enter);
            current_state->exit = temp;
        }
        
        irq_set_exclusive_handler(SIO_IRQ_PROC0, core0_interrupt);
        irq_set_enabled(SIO_IRQ_PROC0, true);
    }
}

void state_cancel()
{
    irq_set_enabled(SIO_IRQ_PROC0, false);
    multicore_reset_core1();
    multicore_launch_core1(core1_entry);
    multicore_fifo_pop_blocking();  

    if(current_state->exit)
        set_state(current_state->exit);
    
    state_flag = CORE_IDLE;
}