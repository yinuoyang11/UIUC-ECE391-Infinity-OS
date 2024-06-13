/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/*
 * i8259_init
 *   DESCRIPTION: Initializes the 8259 Programmable Interrupt Controller (PIC).
 *   INPUTS: None
 *   OUTPUTS: Initializes the PIC for proper interrupt handling.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Configures the PIC to handle interrupts.
 */
void i8259_init(void) {
    master_mask = 0xff; // initialize the mask for master and slave
    slave_mask = 0xff;
    outb(master_mask , MASTER_8259_DATA);   // mask all interrupt
    outb(slave_mask , SLAVE_8259_DATA);

    outb(ICW1 , MASTER_8259_COMMAND);
    outb(ICW2_MASTER , MASTER_8259_DATA);
    outb(ICW3_MASTER , MASTER_8259_DATA);
    outb(ICW4 , MASTER_8259_DATA);

    outb(ICW1 , SLAVE_8259_COMMAND);
    outb(ICW2_SLAVE , SLAVE_8259_DATA);
    outb(ICW3_SLAVE , SLAVE_8259_DATA);
    outb(ICW4 , SLAVE_8259_DATA);

    enable_irq(2);  // slave pic connect to port 2 on master pic
}

/*
 * enable_irq
 *   DESCRIPTION: Enables (unmasks) the specified IRQ line.
 *   INPUTS: irq_num - the IRQ number to be enabled
 *   OUTPUTS: Enables the specified IRQ line in the PIC.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Unmasks the specified IRQ line for handling interrupts.
 */
void enable_irq(uint32_t irq_num) {
    int irq = (irq_num > interrupt_port) ? irq_num - 8 : irq_num;    // if irq_num > 7, minus 8 to it to map to 0-7
    if (irq_num > interrupt_port){
        slave_mask &= 0xff - (1 << irq); // unmask that interrupt
        outb(slave_mask , SLAVE_8259_DATA);
    }
    else{
        master_mask &= 0xff - (1 << irq);
        outb(master_mask , MASTER_8259_DATA);
    }
}

/*
 * disable_irq
 *   DESCRIPTION: Disables (masks) the specified IRQ line.
 *   INPUTS: irq_num - the IRQ number to be disabled
 *   OUTPUTS: Disables the specified IRQ line in the PIC.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Masks the specified IRQ line to prevent handling interrupts.
 */
void disable_irq(uint32_t irq_num) {
    int irq = (irq_num > interrupt_port) ? irq_num - 8 : irq_num; // if irq_num > 7, minus 8 to it to map to 0-7
    if (irq_num > interrupt_port){
        slave_mask |= 1 << irq; // mask that interrupt out
        outb(slave_mask , SLAVE_8259_DATA);
    }
    else{
        master_mask |= 1 << irq;
        outb(master_mask , MASTER_8259_DATA);
    }
}

/*
 * send_eoi
 *   DESCRIPTION: Sends end-of-interrupt (EOI) signal to the PIC(s) after handling an IRQ.
 *   INPUTS: irq_num - the IRQ number for which EOI signal is to be sent
 *   OUTPUTS: Sends EOI signal to the PIC(s) based on the IRQ number.
 *   RETURN VALUE: None
 *   SIDE EFFECTS: Notifies the PIC(s) that interrupt handling for the specified IRQ is complete.
 */
void send_eoi(uint32_t irq_num) {
if (irq_num <= interrupt_port){
    outb(irq_num | EOI , MASTER_8259_COMMAND);
    return;
 }
 outb(irq_num + EOI - 8, SLAVE_8259_COMMAND); // send EOI to both slave and master
 outb(EOI + 2, MASTER_8259_COMMAND);
}
