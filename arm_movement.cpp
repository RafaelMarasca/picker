#include "arm_movement.h"
#include <math.h>
#include <pico/stdlib.h>
#include <stdlib.h>
#include <stdio.h>

static uint64_t prev = true;

arm_t *arm_init(uint16_t gpio1, uint16_t gpio2, uint16_t gpio3, uint16_t gpio4, float angles[4])
{
    arm_t *brobo = (arm_t*)malloc(sizeof(arm_t));

    brobo->m[0] = sg90_init(gpio1);
    brobo->m[1] = sg90_init(gpio2);
    brobo->m[2] = sg90_init(gpio3);
    brobo->m[3] = sg90_init(gpio4);

    for(int i = 0; i < 4; i++)
    {
        sg90_attach(brobo->m[i], angles[i]);
        brobo->prev_angles[i] = angles[i];
    }   
    return brobo;
}

void arm_move_to_angles(arm_t *brobo, float angles[4], int index[4])
{
    int i = 0;

    for(int j = 0; j < 4; j++)
    {
        if(index[j] == 3 /*|| index[j] == 2*/)
        {
            sleep_ms(500);
            sg90_write(brobo->m[index[j]], angles[index[j]]);
            sleep_ms(40);
            brobo->prev_angles[index[j]] = angles[index[j]];
            continue;
        }

        if(brobo->prev_angles[index[j]] < angles[index[j]])
        {
            for(i = brobo->prev_angles[index[j]]; i < (int)angles[index[j]]; i += 10)
            {
                sg90_write(brobo->m[index[j]], (float) i);
                sleep_ms(40);
            }
            sg90_write(brobo->m[index[j]], angles[index[j]]);
            sleep_ms(40);
        }
        else if(brobo->prev_angles[index[j]] > angles[index[j]])
        {
            for(i = brobo->prev_angles[index[j]]; i > (int)angles[index[j]]; i -= 10)
            {
                sg90_write(brobo->m[index[j]], (float) i );
                sleep_ms(40);
            }
            sg90_write(brobo->m[index[j]], angles[index[j]]);
            sleep_ms(40);
        }
        brobo->prev_angles[index[j]] = angles[index[j]];
    }
}

void arm_move_to_origin(arm_t* brobo, float origin[4])
{
    int index [] = {1,2,3,0};
    arm_move_to_angles(brobo, origin, index);
}

void arm_move_joystick(arm_t *brobo, uint16_t x, uint16_t y, uint16_t bz1, uint16_t bz2, uint16_t bg) 
{

    float angles[4];

    for(int i = 0; i < 4; i++)
        angles[i] = brobo->prev_angles[i];

    if(bg && prev)
    {
        prev = false;
        angles[3] = 180 - angles[3];
    }else if(!bg)
    {
        prev = true;
    }

    if(x > 2048 + 500)
    {
        angles[0] = brobo->prev_angles[0] + ((float)(brobo->prev_angles[0] < 179.9))*10.0f;
    }else if(x < 2048 - 500)
    {
        angles[0] = brobo->prev_angles[0] - ((float)(brobo->prev_angles[0] > 0.001))*10.0f;
    }

    if(y > 2048 + 500)
    {
        angles[1] = brobo->prev_angles[1] + ((float)(brobo->prev_angles[1] < 179.9)) * 10.0f;
    }else if(y < 2048 - 500)
    {
        angles[1] = brobo->prev_angles[1] - ((float)(brobo->prev_angles[1] > 0.001)) * 10.0f;
    }

    if(bz2)
    {
        angles[2] = brobo->prev_angles[2] + ((float)(brobo->prev_angles[0] < 179.9))*10.0f;
    }else if(bz1)
    {
        angles[2] = brobo->prev_angles[2] - ((float)(brobo->prev_angles[2] > 0.001)) * 10.0f;
    }
    

    int index [] = {0,1,2,3};

    arm_move_to_angles(brobo, angles, index);
    // float z = brobo->prev_angles->c;

    /*
    int i = 0;
    if(brobo->prev_angles->a < x)
    {
        for(i = brobo->prev_angles->a; i <= x; i += 2)
        {
            sg90_write(brobo->m1, (float) i );
            sleep_ms(20);
        }
        sg90_write(brobo->m1, x);
        sleep_ms(20);
    }
    else if(brobo->prev_angles->a > x)
    {
        for(i = brobo->prev_angles->a; i >= x; i -= 2)
        {
            sg90_write(brobo->m1, (float) i );
            sleep_ms(20);
        }
        sg90_write(brobo->m1, x);
        sleep_ms(20);
    }

    if(brobo->prev_angles->b < y)
    {
        for(int i = brobo->prev_angles->b; i <= y; i += 2)
        {
            sg90_write(brobo->m2, (float) i);
            sleep_ms(20);
        }
        sg90_write(brobo->m2, y);
        sleep_ms(20);
    }    
    else if(brobo->prev_angles->b > y)
    {
        for(int i = brobo->prev_angles->b; i >= y; i -= 2)
        {
            sg90_write(brobo->m2, (float) i);
            sleep_ms(20);
        }
        sg90_write(brobo->m2, y);
        sleep_ms(20);
    }
    */

//     if(bz1 >= 1 && bz2 <= 0)
//         z += 2.0f;
//     else if (bz1 <= 0 && bz2 >= 1)
//         z -= 2.0f;

//     if(z - 180.0f > 0.01f)
//         z = 180.0f;
//     else if(z < 0.0f)
//         z = 0.0f;

        
//    sg90_write(brobo->m1, x);
//    sleep_ms(20);
//    sg90_write(brobo->m2, y);
//    sleep_ms(20);
//    sg90_write(brobo->m3, z);
//    sleep_ms(200);

//     if(isPressed)
//         sg90_write(brobo->m4, 170.0f);
//     else
//         sg90_write(brobo->m4, 0.0f);

//     brobo->prev_angles = float_vec_init(x, y, z);    
}


