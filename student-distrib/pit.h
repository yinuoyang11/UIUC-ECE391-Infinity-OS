#ifndef PIT_H
#define PIT_H

#include "lib.h"
#include "i8259.h"
#define MODE_REGISTER   0x43
#define REGISTER_BITS   0x36
#define BASE_FREQ 1193182
#define SET_FREQ 100
#define CHANNEL_0 0x40
#define CHANNEL_2 0x42
void pit_init();
void pit_handler();

#endif