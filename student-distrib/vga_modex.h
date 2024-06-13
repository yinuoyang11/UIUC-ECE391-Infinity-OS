#ifndef ECE391MODEX_H
#define ECE391MODEX_H

#include "lib.h"
#include "types.h"
#include "system_call.h"
#include "page.h"
#include "data/graphic.h"
#include "data/music.h"
#include "data/terminal_icon.h"
#include "data/desktop.h"
#include "rtc.h"
#include "pit.h"
#define MODEX_VIDEO     0xa0000
#define IMAGE_X_DIM     320   
#define IMAGE_Y_DIM     200
#define IMAGE_X_WIDTH   (IMAGE_X_DIM / 4)          
#define SCROLL_X_DIM    IMAGE_X_DIM                
#define SCROLL_Y_DIM    IMAGE_Y_DIM                
#define SCROLL_X_WIDTH  (IMAGE_X_DIM / 4)          

extern int set_mode_13h();
extern void clear_mode_X();
extern void clear_screens();
extern void open_text_terminal();
void write_font_data();
void fill_text_palette();
extern int mode13_flag;
#endif