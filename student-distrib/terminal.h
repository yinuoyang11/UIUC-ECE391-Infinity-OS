#ifndef TERMINAL_H
#define TERMINAL_H

#include "lib.h"
#include "types.h"
#include "keyboard.h"
#include "scheduler.h"
#define terminal_size   128
#define four_kb 0x1000
#define MAX_HISTORY     20
#define MAX_COMMAND     128

void terminal_init();
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
void terminal_switch(int32_t target_terminal);
void add_command_to_history(char* command);
char* get_previous_command();
char* get_next_command();

typedef struct terminal
{
    char terminal_buf[terminal_size+1];
    int  terminal_buf_idx;
    int  x_;
    int  y_;
    int  key_pressed;
}terminal_t;

extern int cur_terminal;
extern terminal_t terminal[3];
#endif
