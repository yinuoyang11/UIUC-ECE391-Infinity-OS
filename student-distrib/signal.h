#ifndef SIGNAL_H
#define SIGNAL_H
#include "types.h"
#include "lib.h"
#include "system_call.h"
#include "scheduler.h"

#define DIV_ZERO 0
#define SEGFAULT 1
#define INTERRUPT 2
#define ALARM 3
#define USER1 4
#define STOP 5
#define RESUME 6
#define NUM_SIGS 7


typedef struct hwctx{
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t eax;
    uint32_t ds;
    uint32_t es;
    uint32_t fs;
    uint32_t irq;
    uint32_t error_code;
    uint32_t return_address;
    uint32_t cs;
    uint32_t EFLAGS;
    uint32_t ESP;
    uint32_t SS;

}hwctx; 

int32_t generate_signal(uint8_t pid, uint8_t signum); 
int32_t check_signal();
int32_t kill();
int32_t ignore();
int32_t stop();
int32_t resume();
void sigreturn_syscall();
void sigreturn_syscall_end();



#endif