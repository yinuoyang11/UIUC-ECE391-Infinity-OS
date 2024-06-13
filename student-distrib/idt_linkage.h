#ifndef _IDT_LINK_H
#define _IDT_LINK_H
#ifndef ASM
#include "rtc.h"

extern void rtc_handler_link();
extern void keyboard_handler_link();
extern void pit_handler_link();
extern void mouse_handler_link();
extern void systemcall_link();
extern void division_error_link();
extern void debug_link();
extern void NMI_link();
extern void breakpoint_link();
extern void overflow_link();
extern void bd_range_exceeded_link();
extern void invalid_opcode_link();
extern void device_not_available_link();
extern void double_fault_link();
extern void coprocessor_segment_overrun_link();
extern void invalid_tss_link();
extern void segment_not_present_link();
extern void stack_segment_fault_link();
extern void general_protection_fault_link();
extern void page_fault_link();
extern void reserved_1_link();
extern void x87_floating_point_link();
extern void alignment_check_link();
extern void machine_check_link();
extern void simd_floating_point_link();
#endif
#endif
