#include "keyboard.h"
#include "terminal.h"
#include "signal.h"
// key flag
int shift_flag = 0;
int caps_flag = 0;
int ctrl_flag = 0;
int enter_flag = 0;
int alt_flag = 0;
int piano_on = 0;
// scancode set
// four conditions: normal, casp_lock, shift, caps_lock and shift
char scancode_set[4][scan_number] =
{
{0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0,0,
'q','w','e','r','t','y','u','i','o','p','[',']',0,0,
'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',
'z','x','c','v','b','n','m',',','.','/',0,0,0,' ',0},

{0,0,'!','@','#','$','%','^','&','*','(',')','_','+',0,0,
'Q','W','E','R','T','Y','U','I','O','P','{','}',0,0,
'A','S','D','F','G','H','J','K','L',':','"','~',0,'|',
'Z','X','C','V','B','N','M','<','>','?',0,0,0,' ',0},

{0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0,0,
'Q','W','E','R','T','Y','U','I','O','P','[',']',0,0,
'A','S','D','F','G','H','J','K','L',';','\'','`',0,'\\',
'Z','X','C','V','B','N','M',',','.','/',0,0,0,' ',0},

{0,0,'!','@','#','$','%','^','&','*','(',')','_','+',0,0,
'q','w','e','r','t','y','u','i','o','p','{','}',0,0,
'a','s','d','f','g','h','j','k','l',':','"','~',0,'|',
'z','x','c','v','b','n','m','<','>','?',0,0,0,' ',0}
};

char scancodes_piano[26] = {
    0x1E, // A
    0x30, // B
    0x2E, // C
    0x20, // D
    0x12, // E
    0x21, // F
    0x22, // G
    0x23, // H
    0x17, // I
    0x24, // J
    0x25, // K
    0x26, // L
    0x32, // M
    0x31, // N
    0x18, // O
    0x19, // P
    0x10, // Q
    0x13, // R
    0x1F, // S
    0x14, // T
    0x16, // U
    0x2F, // V
    0x11, // W
    0x2D, // X
    0x15, // Y
    0x2C  // Z
};


int32_t note_frequencies[26] = {
    196, 415, 370, 220, 124, 233, 247, 262, 165, 277,
    294, 311, 466, 440, 175, 185, 110, 131, 208, 139,
    156, 392, 117, 349, 147, 330
};
 
// functions
/*
* enable_keyboard
* input  none
* output none
* description: enable keyboard port
*/
void enable_keyboard()  // not used
{
    int data = inb(kb_port);
    outb(kb_port , data & 0x7F);
}
/*
* disable_keyboard
* input  none
* output none
* description: disable keyboard port
*/
void disable_keyboard() // not used
{
    int data = inb(kb_port);
    outb(kb_port , data | 0x80);
}
/*
* keyboard_init
* input  none
* output none
* description: initial keyboard parameters and enable keyboard interrupt
*/
void keyboard_init()
{
    shift_flag = 0; // initial all flags
    caps_flag = 0;
    enter_flag = 0;
    alt_flag = 0;
    // enable_keyboard();
    enable_irq(1);  // enable keyboard interrupt
}
/*
* keyboard_handler
* input  none
* output none
* description: receive a scancode when interrupt, handle that scancode according to different conditions. and print that character on screen
*/
void keyboard_handler()
{
    cli();
    int scancode = inb(kb_data_reg);
    int i,j;
    int position, position_x, position_y;
    char* buf = terminal[cur_terminal].terminal_buf;
    int* idx = &terminal[cur_terminal].terminal_buf_idx;
    switch(scancode) {
        case pgup_press:
            buf=get_previous_command();
            if (buf==NULL || buf[0]=='\0')  break;
            map_vid_to_vid();
            memset(terminal[cur_terminal].terminal_buf,0,128);
            position=terminal[cur_terminal].y_*80+terminal[cur_terminal].x_;
            position=position-terminal[cur_terminal].terminal_buf_idx;
            position_x=position%80;
            position_y=position/80;
            for (i=position;i<position+terminal[cur_terminal].terminal_buf_idx;i++){
            *(uint8_t *)(0xb8000 + (i << 1)) = ' ';
            *(uint8_t *)(0xb8000 + (i << 1) + 1) = 0x7;
            }
            terminal[cur_terminal].x_=position_x;
            terminal[cur_terminal].y_=position_y;
            for (i=0;i<strlen(buf)-1;i++){
                terminal[cur_terminal].terminal_buf[i]=buf[i];
                yyn_putc_pro(buf[i]);
            }
            terminal[cur_terminal].terminal_buf_idx=strlen(buf)-1;
            sched_update_vid_paging(scheduled_terminal, cur_terminal);
            break;
        case pgdn_press:
            buf=get_next_command();
            if (buf==NULL || buf[0]=='\0')  break;
            map_vid_to_vid();
            memset(terminal[cur_terminal].terminal_buf,0,128);
            position=terminal[cur_terminal].y_*80+terminal[cur_terminal].x_;
            position=position-terminal[cur_terminal].terminal_buf_idx;
            position_x=position%80;
            position_y=position/80;
            for (i=position;i<position+terminal[cur_terminal].terminal_buf_idx;i++){
            *(uint8_t *)(0xb8000 + (i << 1)) = ' ';
            *(uint8_t *)(0xb8000 + (i << 1) + 1) = 0x7;
            }
            terminal[cur_terminal].x_=position_x;
            terminal[cur_terminal].y_=position_y;
            for (i=0;i<strlen(buf)-1;i++){
                terminal[cur_terminal].terminal_buf[i]=buf[i];
                yyn_putc_pro(buf[i]);
            }
            terminal[cur_terminal].terminal_buf_idx=strlen(buf)-1;
            sched_update_vid_paging(scheduled_terminal, cur_terminal);
            break;

        case enter:
            // song_main();
            buf[(*idx)++] = '\n';   // handle enter key
            yyn_putc_pro('\n');
            enter_flag = 1;
            key_pressed = enter;
            // terminal[cur_terminal].terminal_buf_idx = 0;    // reset keyboard buffer, may delete after this checkpoint
            // memset(terminal[cur_terminal].terminal_buf, 0, terminal_size + 1);
            terminal[cur_terminal].key_pressed = enter;
            break;
        case left_alt:
            alt_flag = 1;
            break;
        case left_alt_release:
            alt_flag = 0;
            break;
        case f1_press:
            if (alt_flag == 1){
                terminal_switch(0);
            }
            break;
        case f2_press:
            if (alt_flag == 1){
                terminal_switch(1);
            }
            break;
        case f3_press:
            if (alt_flag == 1){
                terminal_switch(2);
            }
            break;
        case left_shift:
            shift_flag = 1;
            key_pressed = left_shift;
            break;
        case caps_lock:
            caps_flag = !caps_flag;
            break;
        case ctrl:
            ctrl_flag = 1;
            break;
        case backspace:
            if (*idx > 0){  // handle backspace
                (*idx)--;
                buf[*idx] = 0;
                kb_backspace();
            }
            break;
        case tab:
            for (i = 0;i < 4;i++){  // print four space
                if ((*idx) + 1 <= kb_buffer_size){
                    buf[*idx] = ' ';
                    yyn_putc_pro(' ');
                    (*idx)++;
                }
            }
            break;
        case left_shift_release:
            shift_flag = 0;
            break;
        case right_shift_release:
            shift_flag = 0;
            break;
        case ctrl_release:
            ctrl_flag = 0;
            break;
        default:
            if (scancode > 0x80){
                turn_off_sound();
                break;
            }
            if (scancode > scan_number) 
            break;  // if out of range return
            if (scancode == 0x26 && ctrl_flag == 1){
                clear();
                break;}
            else if (scancode == 0x2E && ctrl_flag == 1){
                int pid_ = running_process[cur_terminal];
                generate_signal(pid_, INTERRUPT);
                
                break;
            }
            else if (scancode == 0x2C && ctrl_flag == 1){
                int pid_ = running_process[cur_terminal];
                generate_signal(pid_, STOP);
                break;
            }
            else if (scancode == 0x2D && ctrl_flag == 1){
                int pid_ = running_process[cur_terminal];
                generate_signal(pid_, RESUME);
                break;
            }
            if (scancode == 0x10 && ctrl_flag == 1 && piano_on == 0)
            {
                piano_on = 1;
                break;
            }
            if (scancode == 0x10 && ctrl_flag == 1 && piano_on == 1)
            {
                piano_on = 0;
                break;
            }
            if (piano_on){
                for (j=0;j<26;j++){
                    if (scancode == scancodes_piano[j]){
                        play_sound(note_frequencies[j]);
                    }
                }
                break;
            }
            put_char(scancode); 
            break;
    }
    send_eoi(1);    // send end of interrupt signal
    sti();
}


/*
* put_char
* inpu scancode: scancode received from keyboard port
* output none
* description: print a character correspond to that scancode on screen
*/
void put_char(int scancode){
    char c;
    char* buf = terminal[cur_terminal].terminal_buf;    // get current terminal's buffer and buffer index
    int* idx = &terminal[cur_terminal].terminal_buf_idx;
    int scan_set_num = shift_flag + caps_flag * 2;  // calculate scan code set index
    if ((*idx) + 1 > kb_buffer_size)    return;
    c = scancode_set[scan_set_num][scancode];   // get scancode character
    buf[(*idx)] = c;
    (*idx)++;   // write that character in buffer
    yyn_putc_pro(c);
}
