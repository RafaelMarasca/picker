#ifndef GUI_H
#define GUI_H

#include <inttypes.h>
#include "display.h"

#define TEXT_SIZE 230*80*2
#define ICON_SIZE 48*48*2
#define BG_SIZE 240*240*2
#define IMG_SIZE 176*144

typedef struct
{
    const uint8_t* text;
    const uint8_t* icon[2];
    const uint8_t* background;
    const uint8_t* top_arrow;
    const uint8_t* bot_arrow;
    const uint8_t* img;

    uint8_t anim;

    uint8_t text_area[4];
    uint8_t icon_area[4];
    uint8_t img_area[4];

    bool has_next;
    bool has_prev;

    bool is_visible;
    
}gui_t;

gui_t* new_gui(const uint8_t* text, const uint8_t text_area[4], const uint8_t* icon1, const uint8_t* icon2, const uint8_t icon_area[4],
                 const uint8_t* background, bool has_next, bool has_prev);
gui_t* new_gui_img(const uint8_t* img, const uint8_t img_area[4]);
void show_gui(gui_t* gui);
void hide_gui(gui_t* gui);
void init_gui(display_t* display);


#endif //GUI_H