#include "idt.h"
#include "idt_linkage.h"
#include "system_call.h"
#include "types.h"

/* 
 * division_error
 *   DESCRIPTION: Handles the Division error exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of a Division error exception.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Enters an infinite loop after printing the message.
 */
void division_error(){
    cli();
    printf("Exception 0x00: Division error\n");
    halt((uint8_t)255);
    sti();
}

/* 
 * debug
 *   DESCRIPTION: Handles the Debug exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of a Debug exception.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Enters an infinite loop after printing the message.
 */
void debug(){
    cli();
    printf("Exception 0x01: Debug\n");
    halt((uint8_t)255);
    // while(1);
    sti();
}

/* 
 * NMI
 *   DESCRIPTION: Handles the Non-maskable interrupt exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of a Non-maskable interrupt exception.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Enters an infinite loop after printing the message.
 */
void NMI(){
    cli();
    printf("Exception 0x02: Non-maskable interrupt\n");
    halt((uint8_t)255);
    // while(1);
    sti();
}

/* 
 * breakpoint
 *   DESCRIPTION: Handles the Breakpoint exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of a Breakpoint exception.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Enters an infinite loop after printing the message.
 */
void breakpoint(){
    cli();
    printf("Exception 0x03: Breakpoint\n");
    halt((uint8_t)255);
    // while(1);    
    sti();
}

/* 
 * overflow
 *   DESCRIPTION: Handles the Overflow exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of an Overflow exception.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Enters an infinite loop after printing the message.
 */
void overflow(){
    cli();
    printf("Exception 0x04: Overflow\n");
    halt((uint8_t)255);
    // while(1);
    sti();
}


/* 
 * bd_range_exceeded
 *   DESCRIPTION: Handles the Bound range exceeded exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of a Bound range exceeded exception.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Enters an infinite loop after printing the message.
 */
void bd_range_exceeded(){
    cli();
    printf("Exception 0x05: Bound range exceeded\n");
    halt((uint8_t)255);
    // while(1);
    sti();
}

/* 
 * invalid_opcode
 *   DESCRIPTION: Handles the Invalid opcode exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of an Invalid opcode exception.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Enters an infinite loop after printing the message.
 */
void invalid_opcode(){
    cli();
    printf("Exception 0x06: Invalid opcode\n");
    halt((uint8_t)255);
    // while(1);
    sti();
}


/* 
 * device_not_available
 *   DESCRIPTION: Handles the Device not available exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of a Device not available exception.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Enters an infinite loop after printing the message.
 */
void device_not_available(){
    cli();
    printf("Exception 0x07: Device not available\n");
    halt((uint8_t)255);
    // while(1);
    sti();
}


/* 
 * double_fault
 *   DESCRIPTION: Handles the Double fault exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of a Double fault exception.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Enters an infinite loop after printing the message.
 */
void double_fault(){
    cli();
    printf("Exception 0x08: Double fault\n");
    halt((uint8_t)255);
    // while(1);
    sti();
}


/* 
 * coprocessor_segment_overrun
 *   DESCRIPTION: Handles the Coprocessor Segment Overrun exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of a Coprocessor Segment Overrun exception.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Enters an infinite loop after printing the message.
 */
void coprocessor_segment_overrun(){
    cli();
    printf("Exception 0x09: Coprocessor Segment Overrun\n");
    halt((uint8_t)255);
    // while(1);
    sti();
}

/* 
 * invalid_tss
 *   DESCRIPTION: Handles the Invalid TSS exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of an Invalid TSS exception.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Enters an infinite loop after printing the message.
 */
void invalid_tss(){
    cli();
    printf("Exception 0x0A: Invalid TSS\n");
    halt((uint8_t)255);
    // while(1);
    sti();
}


/* 
 * segment_not_present
 *   DESCRIPTION: Handles the Segment not present exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of a Segment not present exception.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Enters an infinite loop after printing the message.
 */

void segment_not_present(){
    cli();
    printf("Exception 0x0B: Segment not present\n");
    halt((uint8_t)255);
    // while(1);
    sti();
}

/* 
 * stack_segment_fault
 *   DESCRIPTION: Handles the Stack segment fault exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of a Stack segment fault exception.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Enters an infinite loop after printing the message.
 */
void stack_segment_fault(){
    cli();
    printf("Exception 0x0C: Stack segment fault\n");
    halt((uint8_t)255);
    // while(1);
    sti();
}

/* 
 * general_protection_fault
 *   DESCRIPTION: Handles the General protection fault exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of a General protection fault exception.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Enters an infinite loop after printing the message.
 */
void general_protection_fault(){
    cli();
    printf("Exception 0x0D: General protection fault\n");
    halt((uint8_t)255);
    // while(1);
    sti();
}

/* 
 * page_fault
 *   DESCRIPTION: Handles the Page fault exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of a Page fault exception.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Enters an infinite loop after printing the message.
 */
void page_fault(){
    cli();
    printf("Exception 0x0E: Page fault\n");
    halt((uint8_t)255);
    // while(1);
    sti();
}

/* 
 * reserved_1
 *   DESCRIPTION: Handles the Reserved exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of a Reserved exception.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Enters an infinite loop after printing the message.
 */
void reserved_1(){
    cli();
    printf("Exception 0x0F: Reserved\n");
    halt((uint8_t)255);
    // while(1);
    sti();
}

/* 
 * x87_floating_point
 *   DESCRIPTION: Handles the x87 floating point exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of an x87 floating point exception.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Enters an infinite loop after printing the message.
 */
void x87_floating_point(){
    cli();
    printf("Exception 0x10: x87 floating point\n");
    halt((uint8_t)255);
    // while(1);
    sti();
}

/* 
 * alignment_check
 *   DESCRIPTION: Handles the Alignment check exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of an Alignment check exception.
 *   RETURN VALUE: None
*/
void alignment_check(){
    cli();
    printf("Exception 0x11: Alignment check\n");
    halt((uint8_t)255);
    // while(1);
    sti();
}

/* 
 * machine_check
 *   DESCRIPTION: Handles the Machine check exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of a Machine check exception.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Enters an infinite loop after printing the message.
 */
void machine_check(){
    cli();
    printf("Exception 0x12: Machine check\n");
    halt((uint8_t)255);
    // while(1);
    sti();
}

/* 
 * simd_floating_point
 *   DESCRIPTION: Handles the SIMD floating-point exception.
 *   INPUTS: None
 *   OUTPUTS: Prints a message indicating the occurrence of a SIMD floating-point exception.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Enters an infinite loop after printing the message.
 */
void simd_floating_point(){
    cli();
    printf("Exception 0x13: SIMD floating-point exception\n");
    halt((uint8_t)255);
    // while(1);
    sti();

}


// /* 
//  * systemcall
//  *   DESCRIPTION: Handles system calls.
//  *   INPUTS: None
//  *   OUTPUTS: Prints a message indicating that a system call was made.
//  *   RETURN VALUE: None
//  *   SIDE EFFECTS: Enters an infinite loop after printing the message.
//  */
// void systemcall(){

//     printf("You make a system call.");
//     while(1);
// }

/* 
 * idt_init
 *   DESCRIPTION: Initializes the Interrupt Descriptor Table (IDT) with exception handlers and system call entry.
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: None
 *   SIDE EFFECTS: 
 *     - Initializes the IDT with exception handlers and system call entry.
 */
void idt_init(){
    int i;
    for (i = 0; i < 255; i++){
        idt[i].val[0] = 0;
        idt[i].val[1] = 0;

        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0;
        idt[i].reserved3 = 0; 
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1; 
        idt[i].size = 1;      
        idt[i].reserved0 = 0;
        idt[i].present = 0;
        /*if it's an exception*/
        if (i < 0x20 && i >= 0){
            idt[i].present = 1;
            idt[i].dpl = 0;
            idt[i].reserved3 = 1;
        }
        /*If it's a system call*/
        if (i == 0x80 ){
            idt[i].dpl = 3;
            idt[i].present = 1;
            idt[i].reserved3 = 1;   //bug!!            
        }
    }

    /*initialize idte for keyboard and rtc*/
    idt[KEYBOARD_VEC].present = 1;
    idt[KEYBOARD_VEC].dpl = 0;

    idt[RTC_VEC].present = 1;
    idt[RTC_VEC].dpl = 0;

    idt[PIT_VEC].present = 1;
    idt[PIT_VEC].dpl = 0;

    idt[MOUSE_VEC].present = 1;
    idt[MOUSE_VEC].dpl = 0;
    
    SET_IDT_ENTRY(idt[0],division_error);
    SET_IDT_ENTRY(idt[1],debug);
    SET_IDT_ENTRY(idt[2],NMI);
    SET_IDT_ENTRY(idt[3],breakpoint);
    SET_IDT_ENTRY(idt[4],overflow);
    SET_IDT_ENTRY(idt[5],bd_range_exceeded);
    SET_IDT_ENTRY(idt[6],invalid_opcode);
    SET_IDT_ENTRY(idt[7],device_not_available);
    SET_IDT_ENTRY(idt[8],double_fault);
    SET_IDT_ENTRY(idt[9],coprocessor_segment_overrun);
    SET_IDT_ENTRY(idt[10],invalid_tss);
    SET_IDT_ENTRY(idt[11],segment_not_present);
    SET_IDT_ENTRY(idt[12],stack_segment_fault);
    SET_IDT_ENTRY(idt[13],general_protection_fault);
    SET_IDT_ENTRY(idt[14],page_fault);
    SET_IDT_ENTRY(idt[15],reserved_1);
    SET_IDT_ENTRY(idt[16],x87_floating_point);
    SET_IDT_ENTRY(idt[17],alignment_check);
    SET_IDT_ENTRY(idt[18],machine_check);
    SET_IDT_ENTRY(idt[19],simd_floating_point);
/*device*/
    SET_IDT_ENTRY(idt[0x20],pit_handler_link);  
    SET_IDT_ENTRY(idt[0x21],keyboard_handler_link);  
    SET_IDT_ENTRY(idt[0x28],rtc_handler_link);
    SET_IDT_ENTRY(idt[0x2C],mouse_handler_link);
/*syscall*/
    SET_IDT_ENTRY(idt[0x80],systemcall_link);

}

