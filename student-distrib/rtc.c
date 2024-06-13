#include "rtc.h"
int counter,minute,second,hour;
// int anime_ticks;
// int32_t int_flag = 0;   //initialize the interrupt flag to 0

/* 
 * rtc_init
 *   DESCRIPTION: Initializes the Real-Time Clock (RTC) and enables the RTC 
 *                interrupt with the default rate of 1024 Hz.
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: None
 *   SIDE EFFECTS: 
 *     - Modifies the RTC configuration to enable the interrupt.
 *     - Enables RTC interrupts on the Programmable Interrupt Controller (PIC).
 */
void rtc_init(){
   /*turn on irq8 with the default 1024 Hz rate*/
   int i;
    for (i = 0; i < NUM_TERMINALS; i++){
        vrtcs[i].count_target = MAX_FREQ/2;
        vrtcs[i].terminal_interrupt_flag = 0;
    }
    counter = 0;
    cli();  //critical section starts		
    outb(RTC_RB,RTC_IDX);		
    char prev = inb(RTC_DATA);	
    outb(RTC_RB,RTC_IDX);		/*select RB*/
    outb(prev | 0x40,RTC_DATA);	/*write previous RB value ORed with 0x40 to RB*/
    enable_irq(RTC_IRQ);  //enable rtc on the PIC
    sti();  //critical section ends
}

/* 
 * rtc_handler
 *   DESCRIPTION: Interrupt handler for the Real-Time Clock (RTC). 
 *                Clears the interrupt flag, sends an end-of-interrupt signal, 
 *                and invokes the test_interrupts function.
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: None
 *   SIDE EFFECTS: 
 *     - Clears the RTC interrupt flag.
 *     - Sends end-of-interrupt signal to the PIC.
 *     - Invokes the test_interrupts function.
 */
void rtc_handler(){
    cli();  //critical section starts
    // test_interrupts();
    // int_flag = 1;   //interrupt received
    outb(RTC_RC,RTC_IDX);	// select RC
    inb(RTC_DATA);	// throw away contents to receive interrupt next time
    int i;
    counter++;
    anime_ticks++;
    music_ticks++;
    for (i = 0; i < NUM_TERMINALS; i++) {
        if (!(counter%vrtcs[i].count_target)) {
            vrtcs[i].terminal_interrupt_flag = 1;  // Set virtual interrupt flag for each terminal
        }
    }
    send_eoi(RTC_IRQ);  //send end-of-interrupt signal
    sti();  //critical section ends
}
/* 
 * rtc_read
 *   DESCRIPTION: Waits for the RTC interrupt to occur before returning.
 *   INPUTS: 
 *     - fd: File descriptor (not used in this implementation).
 *     - buf: Buffer to read data into (not used in this implementation).
 *     - nbytes: Number of bytes to read (not used in this implementation).
 *   OUTPUTS: None
 *   RETURN VALUE: Always returns 0.
 *   SIDE EFFECTS: None
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
    while (!vrtcs[scheduled_terminal].terminal_interrupt_flag);
    cli();
    vrtcs[scheduled_terminal].terminal_interrupt_flag = 0;  // Set virtual interrupt flag for each terminal
    sti();
    return 0;
}

/* 
 * rtc_write
 *   DESCRIPTION: Changes the RTC interrupt frequency based on the value 
 *                provided in the buffer.
 *   INPUTS: 
 *     - fd: File descriptor (not used in this implementation).
 *     - buf: Buffer containing the new frequency value.
 *     - nbytes: Number of bytes to write (assumed to be sizeof(int32_t)).
 *   OUTPUTS: None
 *   RETURN VALUE: 
 *     - 0 on success, indicating that the frequency has been successfully changed.
 *     - -1 on failure, indicating an out-of-range frequency.
 *   SIDE EFFECTS: 
 *     - Changes the RTC interrupt frequency.
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
    int32_t freq_write = *(int32_t*) buf;
    /*error check*/
    // if ((freq_write < 2) || (freq_write > 1024) || 
    // ((freq_write - 1) & freq_write))    return -1; // out of range(2 to 2^10) or not 2's power
    if ((freq_write < 2) || (freq_write > 1024))    return -1; // out of range(2 to 2^10) or not 2's power
    cli();
    if (rtc_change_freq(freq_write) != 0) return -1;   //change the frequency
    sti();
    return 0;
}

/* 
 * rtc_open
 *   DESCRIPTION: Opens the RTC and sets the default interrupt frequency to 2 Hz.
 *   INPUTS: 
 *     - filename: Name of the file to open (not used in this implementation).
 *   OUTPUTS: None
 *   RETURN VALUE: Always returns 0.
 *   SIDE EFFECTS: 
 *     - Sets the default RTC interrupt frequency to 2 Hz.
 */
int32_t rtc_open(const uint8_t* filename){
    cli();
    vrtcs[scheduled_terminal].terminal_interrupt_flag = 0;
    if(rtc_change_freq(2)) return -1; //set the frequency to 2Hz by default
    sti();
    return 0;
}

/* 
 * rtc_close
 *   DESCRIPTION: Closes the RTC.
 *   INPUTS: 
 *     - fd: File descriptor (not used in this implementation).
 *   OUTPUTS: None
 *   RETURN VALUE: Always returns 0.
 *   SIDE EFFECTS: None
 */
int32_t rtc_close(int32_t fd){
    return 0;
}

/* 
 * rtc_change_freq
 *   DESCRIPTION: Changes the RTC interrupt frequency based on the new_freq value.
 *   INPUTS: 
 *     - new_freq: New frequency value to set.
 *   OUTPUTS: None
 *   RETURN VALUE: Always returns 0.
 *   SIDE EFFECTS: 
 *     - Changes the RTC interrupt frequency.
 */
int32_t rtc_change_freq(int32_t new_freq){
    // int32_t rate = 6;   //initialize the rate to 6
    // int32_t dividend_freq = 32768; //initialize the frequency to 2^15

        // vrtcs[scheduled_terminal].virtual_freq = new_freq;
        vrtcs[scheduled_terminal].count_target = 1024 / new_freq;
        // counter = 0;
        // }
    // else
    // while (dividend_freq>>(rate -1) != new_freq){
    //     rate++;
    // }
    // rate &= 0x0F;   //rate is only 4 LSB
    // cli();
    // outb(RTC_RA, RTC_IDX);		// select RA and disable NMI
    // char prev=inb(RTC_DATA);	// get initial value of RA
    // outb(RTC_RA, RTC_IDX);		// select RA and disable NMI
    // outb((prev & 0xF0) | rate, RTC_DATA); //write rate to A.
    // sti();
    return 0;
}

int check_update_in_progress(){
    outb(0x0a,0x70);
    return (inb(0x71) & 0x80);
}
// Function to convert BCD to binary
int bcd_to_bin(int bcd_value) {
    return (bcd_value & 0x0F) + ((bcd_value >> 4) * 10);
}

void rtc_time(){
    int status_b;
    while (check_update_in_progress()){}
    cli();
    // garbage = inb(0x71);
    // Read status register B to check if data is in BCD format
    outb(0x0b, 0x70);
    status_b = inb(0x71);
    outb(0x00,0x70);
    second = inb(0x71);
    outb(0x02,0x70);
    minute = inb(0x71);
    outb(0x04,0x70);
    hour = inb(0x71);    
    sti();

    if (!(status_b & 0x04)) {
        second = bcd_to_bin(second);
        minute = bcd_to_bin(minute);
        hour = bcd_to_bin(hour);
    }
}