#include "ov7670.h"
#include "classifier.h"
#include "picker.h"
#include "input.h"
#include "arm_movement.h"
#include "state.h"
#include <stdio.h>
#include "hardware/flash.h"

#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

extern uint32_t buffer[3];

static mutex_t* camera_mutex;
static arm_t* arm;
float buf[FLASH_PAGE_SIZE/sizeof(float)];
const float *flash_target_contents = (const float *) (XIP_BASE + FLASH_TARGET_OFFSET);

float origin[4] = {90.0f, 180.0f, 180.0f, 0.0f};
float origin_closed[4] = {90.0f, 180.0f, 180.0f, 150.0f};
int pick_index  [] = {0,2,1,3};
int place_index [] = {1,0,2,3};

static float positions[12][4] = {{ 62.0f, 30.0f, 115.0f, 150.0f},
                                 { 60.0f, 60.0f,  45.0f, 150.0f},
                                 { 45.0f, 75.0f,  17.0f, 150.0f},
                                 { 80.0f, 45.0f,  80.0f, 150.0f},
                                 { 75.0f, 65.0f,  35.0f, 150.0f},
                                 { 70.0f, 80.0f,   5.0f, 150.0f},
                                 { 95.0f, 40.0f,  85.0f, 150.0f},
                                 { 95.0f, 65.0f,  33.0f, 150.0f},
                                 {100.0f, 75.0f,   5.0f, 150.0f},
                                 {105.0f, 35.0f, 100.0f, 150.0f},
                                 {110.0f, 57.0f,  40.0f, 150.0f},
                                 {120.0f, 75.0f,  15.0f, 150.0f}};

static float actions[3][4] = {{ 180.0f,180.0f, 180.0f, 0.0f},
                              {  0.0f, 120.0f,  20.0f, 0.0f},
                              {170.0f, 110.0f,  20.0f, 0.0f}};


void picker_init(mutex_t* cam_mutex, uint8_t** capture_ptr)
{
    camera_mutex = cam_mutex;

    ov7670_init();

    *capture_ptr = ov7670_data;

    arm = arm_init(16, 17, 18, 19, origin);
    
}

void picker_auto_pick()
{

    mutex_enter_blocking(camera_mutex);
    sleep_ms(100);
    ov7670_get_frame();
    mutex_exit(camera_mutex);

    classifier_load_image(ov7670_data);

    for(int i = 0; i < 12; i++)
    {
        int inference = classifier_classify();
        printf("%d\n", inference);
        if(inference != 0)
        {
            arm_move_to_angles(arm, positions[i], pick_index);
            sleep_ms(500);
            arm_move_to_origin(arm, origin_closed);
            sleep_ms(500);
            arm_move_to_angles(arm, actions[inference], place_index);
            sleep_ms(500);
            arm_move_to_origin(arm, origin);
        }
    }
}

void picker_get_capture()
{
    mutex_enter_blocking(camera_mutex);
    sleep_ms(100);
    ov7670_get_frame();
    ov7670_print_data();
    mutex_exit(camera_mutex);
    while(1);
}


void picker_classify()
{
    mutex_enter_blocking(camera_mutex);
    sleep_ms(100);
    ov7670_get_frame();
    mutex_exit(camera_mutex);
    classifier_load_image(ov7670_data);
    classifier_classify_all();
}

void picker_free_play()
{
    //float3_t pos = {0.0f, 0.0f, 0.0f};
    while(1)
    {
        if(mutex_enter_timeout_ms(&adc_buffer_mutex, 100))
        {
            arm_move_joystick(arm, buffer[0], buffer[1], get_button(buffer[2]) & ZUP, get_button(buffer[2]) & ZDOWN, get_button(buffer[2]) & ENTER);
            mutex_exit(&adc_buffer_mutex);
        }
        sleep_ms(100);
    }
}

void picker_set_action_1()
{
    while(1)
    {
        if(mutex_enter_timeout_ms(&adc_buffer_mutex, 100))
        {
           // buf[0] = arm->prev_angles->a;
            //buf[1] = arm->prev_angles->b;
            //buf[2] = arm->prev_angles->c;
            //actions[1] = {arm->prev_angles->a, arm->prev_angles->b, arm->prev_angles->c};
            //arm_move_joystick(arm, buffer[0], buffer[1], get_button(buffer[2]) & ZUP, get_button(buffer[2]) & ZDOWN, get_button(buffer[2]) & ENTER);
            mutex_exit(&adc_buffer_mutex);
        }
        sleep_ms(100);
    }
}

void picker_set_action_2()
{
   // float3_t pos = {0.0f, 0.0f, 0.0f};
    while(1)
    {
        if(mutex_enter_timeout_ms(&adc_buffer_mutex, 100))
        {
     //       buf[3] = arm->prev_angles->a;
      //      buf[4] = arm->prev_angles->b;
       //     buf[5] = arm->prev_angles->c;
        //    actions[2] = {arm->prev_angles->a, arm->prev_angles->b, arm->prev_angles->c};
         //   arm_move_joystick(arm, buffer[0], buffer[1], get_button(buffer[2]) & ZUP, get_button(buffer[2]) & ZDOWN, get_button(buffer[2]) & ENTER);
            mutex_exit(&adc_buffer_mutex);
        }
        sleep_ms(100);
    }
}

void picker_save()
{   
    uint32_t interrupts = save_and_disable_interrupts();
    flash_range_erase((PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE), FLASH_SECTOR_SIZE);
    flash_range_program((PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE), (uint8_t*)buf, FLASH_PAGE_SIZE);
    restore_interrupts (interrupts);
}

void picker_reset()
{
    arm_move_to_origin(arm, origin);
}