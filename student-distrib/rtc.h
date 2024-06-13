#ifndef _RTC_H
#define _RTC_H

#include "i8259.h"
#include "types.h"
#include "lib.h"
#include "scheduler.h"
#define RTC_IDX 0x70
#define RTC_DATA 0x71
#define RTC_RA 0x8A
#define RTC_RB 0x8B
#define RTC_RC 0x0C
#define RTC_IRQ 8
#define NUM_TERMINALS 3
#define MAX_FREQ 1024

typedef struct {
    // int32_t counter;
    int32_t count_target;
    int32_t terminal_interrupt_flag;
} virtual_rtc_t;

virtual_rtc_t vrtcs[NUM_TERMINALS];  // Initialize the virtual RTC control

int anime_ticks;
extern int counter,second,minute,hour;
int music_ticks;
/*Initialize the RTC*/
void rtc_init();
/*RTC interrupt handler*/
void rtc_handler();

int32_t rtc_open(const uint8_t* filename);
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t rtc_close(int32_t fd);
int32_t rtc_change_freq(int32_t new_freq);

int32_t rtc_update();
uint8_t read_rtc_reg(uint8_t reg);
uint8_t read_rtc_second();
void rtc_time();
#endif
