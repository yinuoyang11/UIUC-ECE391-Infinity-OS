#include "terminal.h"

int cur_terminal = 0;
terminal_t terminal[3] = {};
char* screen_buffer[3] = {(char*)0xBA000, (char*)0xBB000, (char*)0xBC000};

char command_history[MAX_HISTORY][MAX_COMMAND];
int cur_history_idx=0;
int total_history=0;

void add_command_to_history(char* command)
{
    strcpy(command_history[total_history],command);
    //cur_history_idx=(cur_history_idx+1)%MAX_HISTORY;
    if (total_history<MAX_HISTORY)  total_history++;
    cur_history_idx=total_history;
}
char* get_previous_command()
{
    if (total_history==0)   return NULL;
    cur_history_idx=(cur_history_idx-1+MAX_HISTORY)%MAX_HISTORY;
    if (command_history[cur_history_idx][0]=='\0'){
        cur_history_idx=(cur_history_idx+1)%MAX_HISTORY;
        return NULL;
    }
    return command_history[cur_history_idx];
}
char* get_next_command()
{
    if (total_history==0)   return NULL;
    cur_history_idx=(cur_history_idx+1)%MAX_HISTORY;
    if (command_history[cur_history_idx][0]=='\0'){
        cur_history_idx=(cur_history_idx-1+MAX_HISTORY)%MAX_HISTORY;
        return NULL;
    }
    return command_history[cur_history_idx];
}

/*
* terminal_init
* input none
* output none
* description: initial all terminal buffer, index, screen x and y
*/
void terminal_init()
{
    int i,j;
    for (i = 0 ; i < 3 ; i++){  //initial all 3 terminals
        memset(terminal[i].terminal_buf , 0 , terminal_size + 1);
        // memset(screen_buffer[i], 0, 0x1000);
        for (j = 0;j <25 * 80; j++) {
            *(uint8_t *)(0xba000 + i*(0x1000) + (j << 1)) = ' ';
            *(uint8_t *)(0xba000 + i*(0x1000) + (j << 1) + 1) = 0x7;
        }
        enable_cursor(0, 15);
        terminal[i].terminal_buf_idx = 0;
        terminal[i].x_ = 0;
        terminal[i].y_ = 0;
        update_cursor(terminal[i].x_, terminal[i].y_);
    }
}
/*
* terminal_read
* input none
* output none
* description: read a buffer from keyboard until enter pressed
*/
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes)
{
    int i;
    if (buf == NULL)    return 0;   
    terminal[scheduled_terminal].key_pressed = 0;
    memset(terminal[scheduled_terminal].terminal_buf, 0, terminal_size + 1);  // clear current terminal's buffer and index
    terminal[scheduled_terminal].terminal_buf_idx = 0;    
    while (terminal[scheduled_terminal].key_pressed != enter);    // wait for enter to be pressed
    add_command_to_history(terminal[scheduled_terminal].terminal_buf);
    int read_bytes = (nbytes < terminal[scheduled_terminal].terminal_buf_idx) ? nbytes : terminal[scheduled_terminal].terminal_buf_idx;
    for (i = 0 ; i < read_bytes ; i ++){
        ((char*)buf)[i] = terminal[scheduled_terminal].terminal_buf[i];   // copy current terminal's terminal buffer to input buffer
    }
    terminal[scheduled_terminal].terminal_buf_idx = 0;
    return read_bytes;
}
/*
* terminal_write
* input none
* output none
* description: write the content of a buffer on screen
*/
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes)
{
    int i;
    if (buf == NULL){
        return 0;
    } 
    // int write_bytes = (nbytes < strlen(buf)) ? nbytes : strlen(buf);    // calculate the write bytes
    for (i = 0; i < nbytes; i++){
        putc(((char*)buf)[i]);  // put each character in a buffer on screen
    }
    return nbytes;
}

void terminal_switch(int32_t target_terminal)
{
    if (cur_terminal == target_terminal)   return;
    map_vid_to_vid();
    char* cur_screen_buffer = screen_buffer[cur_terminal];
    char* target_screen_buffer = screen_buffer[target_terminal];
    memcpy(cur_screen_buffer, VIDEO, four_kb);
    cur_terminal = target_terminal;
    memcpy(VIDEO, target_screen_buffer, four_kb);
    update_cursor(terminal[cur_terminal].x_, terminal[cur_terminal].y_);
    sched_update_vid_paging(scheduled_terminal, cur_terminal);
}
