#ifndef MOUSE_H
#define MOUSE_H

#include "types.h"
#include "lib.h"
#include "i8259.h"
#include "rtc.h"
#include "vga_modex.h"
#include "data/mouse_graph.h"
#include "data/graphic.h"
#include "system_call.h"
#define DATA_PORT   0x60
#define COMMAND_PORT    0X64
#define MODE_13H_MEM_SIZE       65536
#define mode13_xdim 320
#define mode13_ydim 200
#define mouse_xdim  10
#define mouse_ydim  10
typedef struct mouse
{
    int32_t time_counter;
    int32_t mouse_x;
    int32_t mouse_y;
    int32_t left_button_pressed;
    int32_t double_click;
}mouse_t;

void set_sample_rate(uint8_t freq);
void enable_aux();
void mouse_init();
void enable_mouse_interrupt();
void mouse_handler();
void update_text_cursor(int pos_x, int pos_y);
void update_graphic_cursor(int pos_x, int pos_y);
void clear_previous_mouse(int pos_x, int pos_y);
extern mouse_t mice;
#endif