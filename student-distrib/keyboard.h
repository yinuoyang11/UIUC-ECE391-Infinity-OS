#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"
#include "lib.h"
#include "i8259.h"
#include "speaker.h"
#include "terminal.h"
// IO port
#define kb_data_reg 0x60
#define kb_cmd_reg  0x64
#define kb_port     0x61
// Scancode 
#define scan_number 0x3B
#define left_alt    0x38
#define left_alt_release    0xB8
#define f1_press    0x3B
#define f2_press    0x3C
#define f3_press    0x3D
#define left_shift  0x2A
#define right_shift 0x36
#define caps_lock   0x3A
#define enter       0x1C
#define backspace   0x0E
#define ctrl        0x1D
#define space       0x39
#define tab         0x0F
#define pgup_press          72
#define pgdn_press          80
#define left_shift_release 0xAA
#define right_shift_release 0xB6
#define caps_lock_release   0xBA
#define ctrl_release    0x9D
// Keyboard functions
void keyboard_init();
void keyboard_handler();
void enable_keyboard();
void disable_keyboard();
void put_char(int scancode);
// keyboard buffer
#define kb_buffer_size 128




int enter_flag;
uint8_t key_pressed;
#endif 

