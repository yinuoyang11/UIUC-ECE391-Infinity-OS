#include "mouse.h"

mouse_t mice;
#define NUM_COLS    80
#define NUM_ROWS    25

uint8_t * vid_mem = (uint8_t*)MODEX_VIDEO;
uint8_t mode_13_screen_buffer[MODE_13H_MEM_SIZE];
void set_sample_rate(uint8_t freq)
{
    outb(0xD4, COMMAND_PORT);
    outb(0xF3, DATA_PORT);
    while((inb(COMMAND_PORT)&2) != 0);
    outb(0xD4, COMMAND_PORT);
    outb(freq, DATA_PORT);
    while((inb(COMMAND_PORT)&2) != 0);  
}

void enable_aux()
{
    while((inb(COMMAND_PORT)&(1<<1)) != 0);
    outb(0xA8, COMMAND_PORT);  
}

void enable_mouse_interrupt()
{
    outb(0x20, COMMAND_PORT);
    while((inb(COMMAND_PORT)&2) != 0);
    uint8_t status = inb(DATA_PORT);
    status = status | 2;
    status = status & (~0x20);
    outb(0x60, COMMAND_PORT);
    outb(status, DATA_PORT);
}

void mouse_init()
{
    enable_aux();
    enable_mouse_interrupt();
    mice.left_button_pressed = 0;
    mice.time_counter = 0;
    mice.mouse_x = NUM_COLS/2;
    mice.mouse_y = NUM_ROWS/2;
    mice.double_click = 0;
    outb(0xD4, COMMAND_PORT);
    outb(0xF4, DATA_PORT);
    set_sample_rate(20);
    enable_irq(12);
}

void mouse_handler()
{
    cli();
    send_eoi(12);
    uint8_t mouse_byte1, mouse_byte2, mouse_byte3;
    int32_t x_movement, y_movement;
    mouse_byte1 = inb(DATA_PORT);
    mouse_byte2 = inb(DATA_PORT);
    mouse_byte3 = inb(DATA_PORT);

    // check overflow
    if ((mouse_byte1 & 0x80) || (mouse_byte1 & 0x40))   return;

    // check button pressed
    if (mouse_byte1 & 1){
        mice.left_button_pressed = 1;
        // time= read_rtc_second();
        if (mice.time_counter == 0) mice.time_counter = counter;
        else{
            if ((counter - mice.time_counter) <= 256) mice.double_click = 1;
            else    mice.double_click = 0;
            mice.time_counter = counter;
        }
    }
    else    mice.left_button_pressed = 0;

    // negative x movement
    if (mouse_byte1 & 0x10){
        x_movement = mouse_byte2 | 0xffffff00;
    }
    else{
        x_movement = mouse_byte2 & 0xffffffff;
    }

    // negative y movement
    if (mouse_byte1 & 0x20){
        y_movement = mouse_byte3 | 0xffffff00;
    }
    else{
        y_movement = mouse_byte3 & 0xffffffff;
    }
    if (mode13_flag==1){
        clear_previous_mouse(mice.mouse_x, mice.mouse_y);
    }
    mice.mouse_x += x_movement;
    mice.mouse_y -= y_movement;
    if (mode13_flag==0){
        if (mice.mouse_x < 0)   mice.mouse_x = 0;
        if (mice.mouse_x >= NUM_COLS)   mice.mouse_x = NUM_COLS-1;
        if (mice.mouse_y < 0)   mice.mouse_y = 0;
        if (mice.mouse_y >= NUM_ROWS)   mice.mouse_y = NUM_ROWS-1;
        update_text_cursor(mice.mouse_x, mice.mouse_y);
    }
    else{
        if ((mice.mouse_x-mouse_xdim/2) < 0)   mice.mouse_x = mouse_xdim/2;
        if ((mice.mouse_x+mouse_xdim/2) >= mode13_xdim) mice.mouse_x = mode13_xdim-mouse_xdim/2;
        if ((mice.mouse_y-mouse_ydim/2) < 0)   mice.mouse_y = mouse_ydim/2;
        if ((mice.mouse_y+mouse_ydim/2) >= mode13_ydim) mice.mouse_y = mode13_ydim-mouse_ydim/2;
        update_graphic_cursor(mice.mouse_x, mice.mouse_y);
    }
    if ((mice.mouse_x >= 10) && (mice.mouse_x <=30) && (mice.mouse_y >= 10) && (mice.mouse_y <= 30) && mice.double_click && mode13_flag){
        mice.double_click = 0;
        execute("music");
        return;
    }
    else if ((mice.mouse_x >= 10) && (mice.mouse_x <=30) && (mice.mouse_y >= 40) && (mice.mouse_y <= 60) && mice.double_click && mode13_flag){
        mice.double_click = 0;
        open_text_terminal();
        return;
    }
    if (mice.double_click && mode13_flag==0){
        uint8_t font_color = counter %16;
        uint8_t bkg_color = 15-font_color;
        redraw_screen((bkg_color<<4)| font_color);
        mice.double_click = 0;
    }
    sti();
}

void update_text_cursor(int pos_x, int pos_y)
{
    int pos = pos_x + pos_y * 80;

    outb(0x0F,0x3D4);
	outb((uint8_t) (pos & 0xFF),0x3D5);
	outb(0x0E,0x3D4);
	outb((uint8_t) ((pos >> 8) & 0xFF),0x3D5);
}

void update_graphic_cursor(int pos_x, int pos_y)
{
    int x,y,offset;
    offset = (pos_x-mouse_xdim/2) + (pos_y-mouse_ydim/2)*320;
    for (y=0;y<mouse_ydim;y++){
        for (x=0;x<mouse_xdim;x++){
            if (mouse_buffer[x+y*mouse_xdim]!=0){
                *(vid_mem + offset + x + y*320) = 12;
            }
            else{
                *(vid_mem + offset + x + y*320) = desktop[offset + x + y*320];
            }
        }
    }
}

void clear_previous_mouse(int pos_x, int pos_y)
{
    int x,y,offset;
    offset = (pos_x-mouse_xdim/2) + (pos_y-mouse_ydim/2)*320;
    for (y=0;y<mouse_ydim;y++){
        for (x=0;x<mouse_xdim;x++){
            *(vid_mem + offset + x + y*320) = desktop[offset + x + y*320];
        }
    }
}