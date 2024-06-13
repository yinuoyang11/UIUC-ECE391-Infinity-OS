#include "signal.h"

// //called when detecting a signal should be generated and sent to kernel. e.g. keyboard contol C
int32_t generate_signal(uint8_t pid, uint8_t signum){
    if(signum < 0 || signum > NUM_SIGS) return -1;
    pcb_t* pcb_; 
    if (signum == INTERRUPT || signum == STOP){
        pcb_ = get_pcb(pid);
    }
    else{
        pcb_ = get_pcb_with_esp(); 
    }
    pcb_->signal_pending[signum] = 1; //indicate a pending signal
    //printf("\n\n Arrived at generate\n\n");
    return 0;
}

int32_t check_signal(){
    pcb_t* pcb_;
    int i;
    pcb_ = get_pcb_with_esp();
    //printf("\n\n AT CHECK!!!!\n\n");
    for(i = 0; i <NUM_SIGS; i++){
        if(pcb_->signal_pending[i] == 1){
            //printf("there are sigs :( \n)"); 
            pcb_->signal_pending[i] = 0;
            if(pcb_->sig_handler_addresses[i] == NULL){
                if (i < ALARM){
                kill();
                // pcb_->sig_handler_addresses[i] = kill;
                }
                else if(i < STOP){
                    ignore();   
                }
                else if(i == STOP){
                    //printf("at stop\n");
                    stop();
                    return 0;
                }
                else{
                    resume();
                    return 0;
                }
            }        
            else{ //must execute in user space
                // uint32_t user_esp = *((uint32_t*)pcb_ + eight_kb - 8); //find previous process's pcb start, current kernel stack should be right on top of it. 
                //                                             // -4 to point to start of current kernel base, -4 again to point to esp saved in iret context. 
                // uint32_t saved_context_addr = ((uint32_t*)pcb_ + eight_kb - sizeof(hwctx));
                // uint32_t hwctx_top; 
                // uint32_t sigreturn_top;
                // uint32_t signum_addr;
                // uint32_t return_addr_addr;
                // // asm volatile("movl %1, %%esp \n
                // //             subl $4, %%esp\n
                // //             movl %%esp, %0"
                // //             : "=r"(sigframe_base)
                // //             : "r"(user_esp));   // make sigframe_base point to base of kernel-setup signal handler stack frame (before calling signal handler)
                
            
                // uint32_t sigreturn_size = sigreturn_syscall_end - sigreturn_syscall; 
                // sigreturn_top = user_esp - sigreturn_size; 
                // hwctx_top = sigreturn_top - sizeof(hwctx); 
                // signum_addr = hwctx_top - 4; // 4 byte aligned address (one address smaller)
                // return_addr_addr = signum_addr -4; //one address smaller
                // memcpy(sigreturn_top, sigreturn_syscall, sigreturn_size); 
                // memcpy(hwctx_top, saved_context_addr, sizeof(hwctx));
                // memcpy(signum_addr, &i, 4);
                // memcpy(return_addr_addr, sigreturn_top, 4);
                // asm volatile("movl %0, %%esp" 
                //             :
                //             : "r"(hwctx_top)); //make esp point to hwctx top
                // pcb_->sig_handler_addresses[i](); //call user defined sighandler
            }
            
        }
    }
    
}

int32_t kill(){
    // pcb_t* pcb_ = get_pcb_with_esp();
    //printf("kill!");
    halt(0);
}

int32_t ignore(){
    return 0;
}

int32_t stop(){
   // int stop_pid = running_process[cur_terminal]; 
    //stopped_terms++;
    //if (stopped_terms < 3){
        terminal_running[cur_terminal] = 0;

    // }
    return 0;
    }

int32_t resume(){
    //int stop_pid = running_process[cur_terminal]; 
    // if(terminal_running[cur_terminal] == 0){
    //     stopped_terms--;
    // }
    terminal_running[cur_terminal] = 1;
    return 0;
    }
