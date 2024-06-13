#include "speaker.h"

// reference: https://wiki.osdev.org/PC_Speaker
void play_sound(uint32_t setfreq) {
 	uint32_t Div;
 	uint8_t tmp;
 
    //Set the PIT frequency of channel 2
 	Div = BASE_FREQ / setfreq;
 	outb(0xb6,MODE_REGISTER);
 	outb((uint8_t) (Div), CHANNEL_2);
 	outb( (uint8_t) (Div >> 8), CHANNEL_2);
 
 	tmp = inb(TIMER2);
  	if (tmp != (tmp | 3)) {
 		outb( tmp | 3,TIMER2);
 	} //set the bit 0 and bit 1 of 0x61 to change postion
 }
 
void turn_off_sound() {
 	uint8_t tmp = inb(TIMER2) & 0xFC; //set bit 0 and bit 1 back to 0
 	outb(tmp,TIMER2);
 }
 

 void play_note_with_rtc(uint32_t frequency, uint32_t duration_ms) {
    int ret,garbage,rtc_freq,ticks,i;
    if (frequency != 0){
        play_sound(frequency);
    }
    else{
        turn_off_sound();
    }
    i = duration_ms/100;
    // for (i = 0; i < 2; i++) {
    //     ret = rtc_read(0,&garbage,4); // Wait for an RTC interrupt (synchronous wait)
    // }
	music_ticks = 0;
	while (music_ticks < 40*i ){};
    turn_off_sound();
}


