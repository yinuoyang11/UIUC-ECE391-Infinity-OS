#ifndef _IDT_H
#define _IDT_H
#include "lib.h"
#include "types.h"
#include "x86_desc.h"

#define PIT_VEC 0x20
#define KEYBOARD_VEC 0x21
#define RTC_VEC 0x28
#define MOUSE_VEC   0x2C
void idt_init();


extern void division_error();
extern void debug();
extern void NMI();
extern void breakpoint();
extern void overflow();
extern void bd_range_exceeded();
extern void invalid_opcode();
extern void device_not_available();
extern void double_fault();
extern void coprocessor_segment_overrun();
extern void invalid_tss();
extern void segment_not_present();
extern void stack_segment_fault();
extern void general_protection_fault();
extern void page_fault();
extern void reserved_1();
extern void x87_floating_point();
extern void alignment_check();
extern void machine_check();
extern void simd_floating_point();
extern void systemcall();

// extern void virtualization_ex();
// extern void control_protection_ex();
// extern void reserved_2();
// extern void hypervisor_injection_ex();
// extern void vmm_communication_ex();
// extern void security_ex();
// extern void reserved_3();

#endif
