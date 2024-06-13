#include "scheduler.h"

int running_process[3] = {NOT_INITIALIZED, NOT_INITIALIZED, NOT_INITIALIZED};
int scheduled_terminal = -1;
volatile int terminal_running[3] = {1, 1, 1};

void scheduler()
{
    // printf("scheduler called\n");
    uint32_t esp_, ebp_, next_esp;
    scheduled_terminal = (scheduled_terminal + 1) % 3;
    while(terminal_running[scheduled_terminal] == 0){
        scheduled_terminal = (scheduled_terminal + 1) % 3;
    }
    int pid_ = running_process[scheduled_terminal];
    asm volatile("mov %%esp, %0" : "=r"(esp_));
    asm volatile("mov %%ebp, %0" : "=r"(ebp_));
    pcb_t* pcb_ = get_pcb_with_esp();
    pcb_->schedule_esp = esp_;
    pcb_->schedule_ebp = ebp_;
    sched_update_vid_paging(scheduled_terminal, cur_terminal);
    //sched_update_user_vid_paging(scheduled_terminal, cur_terminal);
    if (pid_ == NOT_INITIALIZED){
        execute((uint8_t*)"shell");
    }
    sched_update_vid_paging(scheduled_terminal, cur_terminal);
    pcb_t* next_pcb = get_pcb(pid_);
    next_esp = next_pcb->schedule_esp;
    //printf("scheduler called\n");
    asm volatile("mov %0, %%esp" : "=r"(next_esp));
    tss.esp0 = (eight_mb - eight_kb * pid_ - 4);
    tss.ss0 = KERNEL_DS;
    page_user_program(pid_);
    asm volatile(
        "movl %0, %%ebp \n\
        leave           \n\
        ret             \n\
        "
    : 
    : "r" (next_pcb->schedule_ebp) 
    );
}
