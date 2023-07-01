#include "display.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "state.h"
#include "gui.h"
#include "images.h"
#include "ov7670.h"
#include "pico/multicore.h"
#include "pico/sync.h"
#include "picker.h"
#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include "input.h"
#include "arm_movement.h"

static uint8_t* capture; 

static mutex_t camera_mutex;

void generate_menus()
{

    init_state();

    uint8_t text_area[4] = {79,158,4,233};
    uint8_t icon_area[4] = {94,141,21,68};
    uint8_t wait_icon_area[4] = {105,152,98,145};
    uint8_t capture_area[4] = {48, 191, 32, 207};
    
    gui_t* auto_gui = new_gui(auto_text, text_area, auto_icon_1,  auto_icon_2, icon_area, back_1, true, false);
    gui_t* play_gui = new_gui(manual_text, text_area, manual_icon_1, manual_icon_2, icon_area, back_1, true, true);
    gui_t* capture_gui = new_gui(capture_text, text_area, capture_icon, capture_icon, icon_area, back_1, false, true);
    //gui_t* action1_gui = new_gui(action1_text, text_area, open, closed, icon_area, back_1, true, true);
   // gui_t* action2_gui = new_gui(action2_text, text_area, open, closed, icon_area, back_1, false, true);
    gui_t* wait_gui = new_gui(NULL, text_area, wait_icon_1, wait_icon_2, wait_icon_area, back_2, false, false);
    gui_t* img_gui = new_gui_img(capture, capture_area);

    
    state_t* auto_state = new_state(auto_gui, NULL);
    state_t* play_state = new_state(play_gui, NULL);
    state_t* capture_state = new_state(capture_gui, NULL);
    //state_t* action1_state = new_state(action1_gui, NULL);
    //state_t* action2_state = new_state(action2_gui, NULL);
    state_t* wait_state = new_state(wait_gui, NULL);
    state_t* img_state = new_state(img_gui, NULL);

    auto_state->next = play_state;
    auto_state->enter = wait_state;
    auto_state->func = picker_auto_pick;

    play_state->next = capture_state;
    play_state->prev = auto_state;
    play_state->func = picker_free_play; 
    play_state->enter = wait_state;   
    
    //capture_state->next = action1_state;
    capture_state->prev = play_state;
    capture_state->enter = img_state;
    capture_state->func = picker_get_capture;

    // action1_state->next = action2_state;
    // action1_state->prev = capture_state;
    // action1_state->func = picker_set_action_1;
    // action1_state->enter = wait_state;

    // action2_state->prev = action1_state;
    // action2_state->func = picker_set_action_2;
    // action2_state->enter = wait_state;

    set_state(auto_state);
}

static float positions[12][4] = {{ 62.0f, 30.0f, 115.0f, 180.0f},
                                 { 60.0f, 65.0f,  45.0f, 180.0f},
                                 { 45.0f, 75.0f,  17.0f, 180.0f},
                                 { 80.0f, 45.0f,  80.0f, 180.0f},
                                 { 75.0f, 65.0f,  35.0f, 180.0f},
                                 { 70.0f, 80.0f,   5.0f, 180.0f},
                                 { 95.0f, 45.0f,  85.0f, 180.0f},
                                 { 95.0f, 70.0f,  33.0f, 180.0f},
                                 {100.0f, 80.0f,   5.0f, 180.0f},
                                 {105.0f, 35.0f, 100.0f, 180.0f},
                                 {115.0f, 65.0f,  45.0f, 180.0f},
                                 {120.0f, 75.0f,  15.0f, 180.0f}};

static float actions[3][4] = {{  0.0f, 0.0f, 0.0f, 0.0f},
                              {  0.0f, 120.0f, 20.0f, 0.0f},
                              {170.0f, 110.0f, 20.0f, 0.0f}};

int main()
{
    stdio_init_all();

    mutex_init(&camera_mutex);
    
    picker_init(&camera_mutex, &capture);

    init_input();
    generate_menus();

    multicore_launch_core1(core1_entry);
    multicore_fifo_pop_blocking();  

    uint8_t input;

    while(1)
    {
        input = handle_input();

        if(input & CANCEL)
        {
            state_cancel();
            picker_reset();
        }

        if(input & UP)
            set_prev_state();
        else if (input & DOWN)
            set_next_state();
        else if (input & ENTER)
            state_act();

        if(mutex_enter_timeout_ms(&camera_mutex, 50))
        {
            run_state();
            mutex_exit(&camera_mutex);
            sleep_ms(150);
        }else
        {
            sleep_ms(200);
        }
    }
}

