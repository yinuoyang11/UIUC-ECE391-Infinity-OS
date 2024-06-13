#ifndef SPEAKER_H
#define SPEAKER_H
#include "types.h"
#include "lib.h"
#include "pit.h"
#include "rtc.h"
#define TIMER2 0x61
// Note structure
typedef struct {
    uint32_t frequency; // Frequency in Hz
    uint32_t duration;       // Duration in milliseconds
} Note;
extern void turn_off_sound();
extern void play_sound(uint32_t nFrequence);
extern void play_note_with_rtc(uint32_t frequency, uint32_t duration_ms);
#endif