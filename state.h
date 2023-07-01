#ifndef STATE_H
#define STATE_H

#include <stdint.h>
#include "gui.h"



typedef struct state
{
    void (*func)();

    struct state* next;
    struct state* prev;
    struct state* enter;
    struct state* exit;
    gui_t* gui;

}state_t;

state_t* new_state(gui_t* gui, void (*func)());

void init_state();

void run_state();

void set_state(state_t* state);

void set_next_state();

void set_prev_state();

void state_act();

void state_cancel();

#endif //STATE_H