#include <stdlib.h>
#include "display.h"
#include "gui.h"
#include "images.h"

static display_t* display;

static const uint8_t bg_area [4] = {0, 239, 0, 239};
static const uint8_t top_arrow_area [2][4] = {{27,74,97,144},{18,65,97,144}};
static const uint8_t bot_arrow_area [2][4]= {{165, 212, 97, 144},{174, 221, 97, 144}};


void init_gui(display_t* new_display)
{
    display = new_display;
}

gui_t* new_gui(const uint8_t* text, const uint8_t text_area[4], const uint8_t* icon1, const uint8_t* icon2, const uint8_t icon_area[4],
                 const uint8_t* background, bool has_next, bool has_prev)
{
    gui_t* gui = (gui_t*)malloc(sizeof(gui_t));
    
    if(gui)
    {
        gui->img = NULL;
        gui->text = text;
        gui->icon[0] = icon1;
        gui->icon[1] = icon2;
        gui->background = background;
        gui->top_arrow = top_arrow;
        gui->bot_arrow = bot_arrow;
        gui->anim = 0;
        gui->is_visible = false;

        for(int i = 0; i < 4; i++)
        {
            gui->text_area[i] = text_area[i];
            gui->icon_area[i] = icon_area[i];
        }

        gui->has_next = has_next;
        gui->has_prev = has_prev;
    }

    return gui;
}

gui_t* new_gui_img(const uint8_t* img, const uint8_t img_area[4])
{
      gui_t* gui = (gui_t*)malloc(sizeof(gui_t));
    
    if(gui)
    {
        gui->img = img;
        
        for(int i = 0; i < 4; i++)
            gui->img_area[i] = img_area[i];

        gui->text = NULL;
        gui->icon[0] = NULL;
        gui->icon[1] = NULL;
        gui->background = NULL;
        gui->top_arrow = NULL;
        gui->bot_arrow = NULL;
        gui->anim = 0;
        gui->is_visible = false;
        gui->has_next = false;
        gui->has_prev = false;
    }

    return gui;

}

void show_gui(gui_t* gui)
{
    if(gui)
    {
        if(!gui->is_visible)
        {
            if(gui->background)
                display_partial(display, bg_area, gui->background, BG_SIZE);

            if(gui->text)
                display_partial(display, gui->text_area, gui->text, TEXT_SIZE);
            
            if(gui->img)
                display_partial_gray(display, gui->img_area, gui->img, IMG_SIZE);

            gui->is_visible = true;
        }

        if(gui->icon[0])
            display_partial(display, gui->icon_area, gui->icon[gui->anim], ICON_SIZE);

        if (gui->has_next)
        {
            clear_partial(display, bot_arrow_area[(gui->anim + 1) % 2], ICON_SIZE);
            display_partial(display, bot_arrow_area[gui->anim], gui->bot_arrow, ICON_SIZE);
        }

        if (gui->has_prev)
        {
            clear_partial(display, top_arrow_area[(gui->anim + 1) % 2], ICON_SIZE);
            display_partial(display, top_arrow_area[gui->anim], gui->top_arrow, ICON_SIZE);
        }

        gui->anim = (gui->anim + 1) % 2;

    }
}

void hide_gui(gui_t* gui)
{
    gui->is_visible = false;
}