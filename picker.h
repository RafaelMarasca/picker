#ifndef PICKER_H
#define PICKER_H

#include "pico/stdlib.h"
#include "pico/sync.h"

void picker_init(mutex_t* cam_mutex, uint8_t** capture_ptr);

void picker_auto_pick();

void picker_get_capture();

void picker_free_play();

void picker_classify();

void picker_save();

void picker_set_action_1();

void picker_set_action_2();

void picker_reset();

#endif //PICKER_H