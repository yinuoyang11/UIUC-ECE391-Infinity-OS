#define ASM 1

#include "idt_linkage.h"

.global rtc_handler_link
.global keyboard_handler_link
.global mouse_handler_link
.global systemcall_link
.global pit_handler_link
.global division_error_link
.global debug_link
.global NMI_link
.global breakpoint_link
.global overflow_link
.global bd_range_exceeded_link
.global invalid_opcode_link
.global device_not_available_link
.global double_fault_link
.global coprocessor_segment_overrun_link
.global invalid_tss_link
.global segment_not_present_link
.global stack_segment_fault_link
.global general_protection_fault_link
.global page_fault_link
.global reserved_1_link
.global x87_floating_point_link
.global alignment_check_link
.global machine_check_link
.global simd_floating_point_link

systemcall_jump:
    .long 0         
    .long halt
    .long execute
    .long read
    .long write
    .long open
    .long close
    .long getargs
    .long vidmap
    .long 0
    .long 0
    .long 0
    .long 0
    .long 0
    .long song_main 
    .long show_time
    .long touch

systemcall_link:
    addl $-4,%esp
    
    pushl %ebp
    pushl %edi
    pushl %esi 
    pushl %edx 
    pushl %ecx 
    pushl %ebx 

    cmpl    $1,%eax
    jl  invalid_number
    cmpl    $16,%eax
    jg  invalid_number

    call *systemcall_jump(,%eax,4)
    jmp systemcall_done

invalid_number:
    movl $-1,%eax

systemcall_done:
    addl $12,%esp
    popl %esi
    popl %edi
    popl %ebp
    addl $4,%esp 
    iret  

rtc_handler_link:
    pushfl
    pushal
    call rtc_handler
    call check_signal
    popal
    popfl
    iret

keyboard_handler_link:
    pushfl
    pushal
    call keyboard_handler
    call check_signal
    popal
    popfl
    iret

mouse_handler_link:
    pushfl
    pushal
    call mouse_handler
    call check_signal
    popal
    popfl
    iret

pit_handler_link:
    pushfl
    pushal
    call pit_handler
    call check_signal
    popal
    popfl
    iret

division_error_link:
    pushfl
    pushal
    call division_error
    call check_signal
    popal
    popfl
    iret
    
debug_link:
    pushfl
    pushal
    call debug
    call check_signal
    popal
    popfl
    iret

NMI_link:
    pushfl
    pushal
    call NMI
    call check_signal
    popal
    popfl
    iret

breakpoint_link:
    pushfl
    pushal
    call breakpoint
    call check_signal
    popal
    popfl
    iret

overflow_link:
    pushfl
    pushal
    call overflow
    call check_signal
    popal
    popfl
    iret

bd_range_exceeded_link:
    pushfl
    pushal
    call bd_range_exceeded
    call check_signal
    popal
    popfl
    iret

invalid_opcode_link:
    pushfl
    pushal
    call invalid_opcode
    call check_signal
    popal
    popfl
    iret

device_not_available_link:
    pushfl
    pushal
    call device_not_available
    call check_signal
    popal
    popfl
    iret

double_fault_link:
    pushfl
    pushal
    call double_fault
    call check_signal
    popal
    popfl
    iret

coprocessor_segment_overrun_link:
    pushfl
    pushal
    call coprocessor_segment_overrun
    call check_signal
    popal
    popfl
    iret

invalid_tss_link:
    pushfl
    pushal
    call invalid_tss
    call check_signal
    popal
    popfl
    iret

segment_not_present_link:
    pushfl
    pushal
    call segment_not_present
    call check_signal
    popal
    popfl
    iret

stack_segment_fault_link:
    pushfl
    pushal
    call stack_segment_fault
    call check_signal
    popal
    popfl
    iret

general_protection_fault_link:
    pushfl
    pushal
    call general_protection_fault
    call check_signal
    popal
    popfl
    iret

page_fault_link:
    pushfl
    pushal
    call page_fault
    call check_signal
    popal
    popfl
    iret

reserved_1_link:
    pushfl
    pushal
    call reserved_1
    call check_signal
    popal
    popfl
    iret

x87_floating_point_link:
    pushfl
    pushal
    call x87_floating_point
    call check_signal
    popal
    popfl
    iret

alignment_check_link:
    pushfl
    pushal
    call alignment_check
    call check_signal
    popal
    popfl
    iret

machine_check_link:
    pushfl
    pushal
    call machine_check
    call check_signal
    popal
    popfl
    iret

simd_floating_point_link:
    pushfl
    pushal
    call simd_floating_point
    call check_signal
    popal
    popfl
    iret
