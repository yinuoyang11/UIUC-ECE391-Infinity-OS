#include "pit.h"

void pit_init()
{
    outb(REGISTER_BITS, MODE_REGISTER);
    outb(BASE_FREQ/SET_FREQ & 0xFF, CHANNEL_0); // Set low byte of frequency
    outb((BASE_FREQ/SET_FREQ >> 8 & 0xFF) , CHANNEL_0); // Set high byte of frequency
    enable_irq(0);
}

void pit_handler()
{
    send_eoi(0);
    scheduler();
}