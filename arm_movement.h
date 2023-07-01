#ifndef ARM_MOVEMENT_H
#define ARM_MOVEMENT_H
#include "sg90.h"

#define RAD2DEG 180.0f*M_1_PI

typedef struct {
    sg90_t* m[4];

    float prev_angles[4];
    //float3_t *size;
}arm_t;

arm_t *arm_init(uint16_t gpio1, uint16_t gpio2, uint16_t gpio3, uint16_t gpio4, float angles[4]);
void arm_move_to_angles(arm_t *brobo, float angles[4], int index[4]);
void arm_move_to_origin(arm_t* brobo, float origin[4]);
void arm_move_joystick(arm_t *brobo, uint16_t x, uint16_t y, uint16_t bz1, uint16_t bz2, uint16_t bg);

#endif //ARM_MOVEMENT_