#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "system_call.h"
#include "page.h"
#include "terminal.h"
#include "types.h"
#define NOT_INITIALIZED  -1
void scheduler();

extern int running_process[3];
extern int scheduled_terminal;
extern volatile int terminal_running[3];
#endif