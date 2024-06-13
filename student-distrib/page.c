#include "page.h"

/* 
 * page_init
 *   DESCRIPTION: Initializes page directory and page tables for paging.
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: None
 *   SIDE EFFECTS: 
 *     - Initializes page directory and page tables for paging.
 *     - Enables paging by setting appropriate control registers (CR0 and CR3).
 */
void page_init(){
    int i = 0; 
    for(i=0; i<1024;i++){
        page_dir[i].MB.content = 0; 
    }   //initialize all 1024 PDEs to 0

    for(i=0; i<1024;i++){
        page_table[i].content = 0; 
    }//initialize all 1024 PTEs to 0

//paging for video mem
page_dir[0].KB.addr = ((uint32_t)page_table) / 4096; //set addr to higher 20 bits of page table address(/2^12)
page_dir[0].KB.p = 1;
page_dir[0].KB.rw = 1;
page_dir[0].KB.us = 1;
page_table[VIDEO >> 12].addr = (VIDEO / 4096); //addr=higher 20 bits of video memory address(/2^12)
page_table[VIDEO >> 12].p = 1; //present=1
page_table[VIDEO >> 12].rw = 1;
page_table[VIDEO >> 12].us = 0;

//kernel page
page_dir[1].MB.p = 1; //present=1
page_dir[1].MB.rw = 1;
page_dir[1].MB.ps = 1;
page_dir[1].MB.us = 0;
page_dir[1].MB.addr_l = (KERNEL_PAGE >> 22 ); //bits 31-22 of kernel page address(4MB/ 0x400000) bitmask:0xFFC000000

//video buffer for 3 terminals
page_dir[TERM1_VID >> 22].KB.addr = ((uint32_t)page_table) / 4096; //set addr to higher 20 bits of page table address(/2^12)
page_dir[TERM1_VID >> 22].KB.p = 1;
page_dir[TERM1_VID >> 22].KB.rw = 1;
page_dir[TERM1_VID >> 22].KB.us = 1;
page_table[(TERM1_VID & 0x003ff000) >> 12].addr = (TERM1_VID / 4096); //addr=higher 20 bits of video memory address(/2^12)
page_table[(TERM1_VID & 0x003ff000) >> 12].p = 1; //present=1
page_table[(TERM1_VID & 0x003ff000) >> 12].rw = 1;
page_table[(TERM1_VID & 0x003ff000) >> 12].us = 0;

page_dir[TERM2_VID >> 22].KB.addr = ((uint32_t)page_table) / 4096; //set addr to higher 20 bits of page table address(/2^12)
page_dir[TERM2_VID >> 22].KB.p = 1;
page_dir[TERM2_VID >> 22].KB.rw = 1;
page_dir[TERM2_VID >> 22].KB.us = 1;
page_table[(TERM2_VID & 0x003ff000) >> 12].addr = (TERM2_VID / 4096); //addr=higher 20 bits of video memory address(/2^12)
page_table[(TERM2_VID & 0x003ff000) >> 12].p = 1; //present=1
page_table[(TERM2_VID & 0x003ff000) >> 12].rw = 1;
page_table[(TERM2_VID & 0x003ff000) >> 12].us = 0;

page_dir[TERM3_VID >> 22].KB.addr = ((uint32_t)page_table) / 4096; //set addr to higher 20 bits of page table address(/2^12)
page_dir[TERM3_VID >> 22].KB.p = 1;
page_dir[TERM3_VID >> 22].KB.rw = 1;
page_dir[TERM3_VID >> 22].KB.us = 1;
page_table[(TERM3_VID & 0x003ff000) >> 12].addr = (TERM3_VID / 4096); //addr=higher 20 bits of video memory address(/2^12)
page_table[(TERM3_VID & 0x003ff000) >> 12].p = 1; //present=1
page_table[(TERM3_VID & 0x003ff000) >> 12].rw = 1;
page_table[(TERM3_VID & 0x003ff000) >> 12].us = 0;


//enable paging 
//asm adapted from OSDev code
    asm volatile(       
        "movl  %0, %%ebx;"  //page address load in to %ebx, then to cr3
        "movl  %%ebx, %%cr3;"
        "movl  %%cr4, %%ecx; " //set PSE bit of cr4 to enable 4MB pages
        "orl   $0x00000010, %%ecx;" 
        "movl  %%ecx, %%cr4;"
        "movl  %%cr0, %%ebx;" //set PG and PE of cr0
        "orl   $0x80000001, %%ebx;"
        "movl  %%ebx, %%cr0;"        //set bit 31 of CR0 to 1 （$0x80000000）, enable paging
        : /*no output*/     //output
        : "r" (&page_dir)   //input
        : "%ebx", "%ecx"           //clobber list
    );
}

/* 
 * page_user_program
 *   DESCRIPTION: Maps user program pages (4MB page for each user program)
 *   INPUTS: pid- process id, using it to calculate where this program's memory is located in physical memory
 *   OUTPUTS: None
 *   RETURN VALUE: None
 *   SIDE EFFECTS: 
 *     - populates some page directory entries
 *     - flushes TLB
 */
void page_user_program(uint32_t pid){
    page_dir[USER_VIRTUAL_MEM >> 22].MB.p = 1; //present=1
    page_dir[USER_VIRTUAL_MEM >> 22].MB.rw = 1;
    page_dir[USER_VIRTUAL_MEM >> 22].MB.ps = 1;
    page_dir[USER_VIRTUAL_MEM >> 22].MB.us = 1;
    page_dir[USER_VIRTUAL_MEM >> 22].MB.addr_l = ((USER_SPACE_START + pid * four_mb) >> 22 ); 
    flush_tlb();
}

//update upon a context switch (scheduling), for terminal writes (kernel privilege!!!!)
void sched_update_vid_paging(uint8_t scheduled_term, uint8_t viewed_term){
    if (scheduled_term == viewed_term){
        // map virtual vid mem to physical vid mem: running program writes directlt to screen
        page_dir[0].KB.addr = ((uint32_t)page_table) / 4096; //set addr to higher 20 bits of page table address(/2^12)
        page_dir[0].KB.p = 1;
        page_dir[0].KB.rw = 1;
        page_dir[0].KB.us = 1;
        page_table[VIDEO >> 12].addr = (VIDEO / 4096); //addr=higher 20 bits of video memory address(/2^12)
        page_table[VIDEO >> 12].p = 1; //present=1
        page_table[VIDEO >> 12].rw = 1;
        page_table[VIDEO >> 12].us = 0;

        page_dir[user_vid_mem >> 22].KB.addr = ((uint32_t)page_table_2) / 4096; //set addr to higher 20 bits of page table address(/2^12)
        page_dir[user_vid_mem >> 22].KB.p = 1;
        page_dir[user_vid_mem >> 22].KB.ps = 0;
        page_dir[user_vid_mem >> 22].KB.rw = 1;
        page_dir[user_vid_mem >> 22].KB.us = 1; //user privilege
        page_table_2[(user_vid_mem & 0x003ff000) >> 12].addr = (VIDEO / 4096); //addr=higher 20 bits of video memory address(/2^12)
        page_table_2[(user_vid_mem & 0x003ff000) >> 12].p = 1; //present=1
        page_table_2[(user_vid_mem & 0x003ff000) >> 12].rw = 1;
        page_table_2[(user_vid_mem & 0x003ff000) >> 12].us = 1;
    }
    else{
        //map virtual vid mem to corresponding background buffer
        page_dir[VIDEO >> 22].KB.addr = ((uint32_t)page_table) / 4096; //set addr to higher 20 bits of page table address(/2^12)
        page_dir[VIDEO >> 22].KB.p = 1;
        page_dir[VIDEO >> 22].KB.rw = 1;
        page_dir[VIDEO >> 22].KB.us = 1;
        page_table[VIDEO >> 12].addr = ((VIDEO + (2 + scheduled_term) * four_kb) / 4096); //addr=higher 20 bits of video memory address(/2^12)
        page_table[VIDEO >> 12].p = 1; //present=1
        page_table[VIDEO >> 12].rw = 1;
        page_table[VIDEO >> 12].us = 0;

        page_dir[user_vid_mem >> 22].KB.addr = ((uint32_t)page_table_2) / 4096; //set addr to higher 20 bits of page table address(/2^12)
        page_dir[user_vid_mem >> 22].KB.p = 1;
        page_dir[user_vid_mem >> 22].KB.ps = 0;
        page_dir[user_vid_mem >> 22].KB.rw = 1;
        page_dir[user_vid_mem >> 22].KB.us = 1; //user privilege
        page_table_2[(user_vid_mem & 0x003ff000) >> 12].addr = ((VIDEO + (2 + scheduled_terminal) * four_kb) / 4096); //addr=higher 20 bits of video memory address(/2^12)
        page_table_2[(user_vid_mem & 0x003ff000) >> 12].p = 1; //present=1
        page_table_2[(user_vid_mem & 0x003ff000) >> 12].rw = 1;
        page_table_2[(user_vid_mem & 0x003ff000) >> 12].us = 1;
    }
    flush_tlb();
}

void sched_update_user_vid_paging(uint8_t scheduled_term, uint8_t viewed_term){
    if (cur_terminal == scheduled_terminal){
        page_dir[user_vid_mem >> 22].KB.addr = ((uint32_t)page_table_2) / 4096; //set addr to higher 20 bits of page table address(/2^12)
        page_dir[user_vid_mem >> 22].KB.p = 1;
        page_dir[user_vid_mem >> 22].KB.ps = 0;
        page_dir[user_vid_mem >> 22].KB.rw = 1;
        page_dir[user_vid_mem >> 22].KB.us = 1; //user privilege
        page_table_2[(user_vid_mem & 0x003ff000) >> 12].addr = (VIDEO / 4096); //addr=higher 20 bits of video memory address(/2^12)
        page_table_2[(user_vid_mem & 0x003ff000) >> 12].p = 1; //present=1
        page_table_2[(user_vid_mem & 0x003ff000) >> 12].rw = 1;
        page_table_2[(user_vid_mem & 0x003ff000) >> 12].us = 1;
    }
    else{
        page_dir[user_vid_mem >> 22].KB.addr = ((uint32_t)page_table_2) / 4096; //set addr to higher 20 bits of page table address(/2^12)
        page_dir[user_vid_mem >> 22].KB.p = 1;
        page_dir[user_vid_mem >> 22].KB.ps = 0;
        page_dir[user_vid_mem >> 22].KB.rw = 1;
        page_dir[user_vid_mem >> 22].KB.us = 1; //user privilege
        page_table_2[(user_vid_mem & 0x003ff000) >> 12].addr = ((VIDEO + (2 + scheduled_terminal) * four_kb) / 4096); //addr=higher 20 bits of video memory address(/2^12)
        page_table_2[(user_vid_mem & 0x003ff000) >> 12].p = 1; //present=1
        page_table_2[(user_vid_mem & 0x003ff000) >> 12].rw = 1;
        page_table_2[(user_vid_mem & 0x003ff000) >> 12].us = 1;
    }
}

//provide access to physical vid mem upon switching terminal view
void map_vid_to_vid(){
    page_dir[0].KB.addr = ((uint32_t)page_table) / 4096; //set addr to higher 20 bits of page table address(/2^12)
    page_dir[0].KB.p = 1;
    page_dir[0].KB.rw = 1;
    page_dir[0].KB.us = 1;
    page_table[VIDEO >> 12].addr = (VIDEO / 4096); //addr=higher 20 bits of video memory address(/2^12)
    page_table[VIDEO >> 12].p = 1; //present=1
    page_table[VIDEO >> 12].rw = 1;
    page_table[VIDEO >> 12].us = 0;
    flush_tlb();
}

void map_mode13h(){
    int i;
    for(i = 0; i < modex_vid_size_4kb; i++){
        page_dir[(mode13h_vid_mem + i*four_kb) >> 22].KB.addr = ((uint32_t)page_table) / 4096; //set addr to higher 20 bits of page table address(/2^12)
        page_dir[(mode13h_vid_mem + i*four_kb) >> 22].KB.p = 1;
        page_dir[(mode13h_vid_mem + i*four_kb) >> 22].KB.ps = 0;
        page_dir[(mode13h_vid_mem + i*four_kb) >> 22].KB.rw = 1;
        page_dir[(mode13h_vid_mem + i*four_kb) >> 22].KB.us = 0; //kernel privilege
        page_table[((mode13h_vid_mem + i * four_kb) & 0x003ff000) >> 12].addr = ((mode13h_vid_mem + i*four_kb) / 4096); //addr=higher 20 bits of video memory address(/2^12)
        page_table[((mode13h_vid_mem + i * four_kb) & 0x003ff000) >> 12].p = 1; //present=1
        page_table[((mode13h_vid_mem + i * four_kb) & 0x003ff000) >> 12].rw = 1;
        page_table[((mode13h_vid_mem + i * four_kb) & 0x003ff000) >> 12].us = 0;

    }
    flush_tlb();
}
/* 
 * flush_tlb
 *   DESCRIPTION: flush_tlb by reloading cr3
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: None
 *   SIDE EFFECTS: 
 *     - clobbers %eax
 *     
 */
void flush_tlb() {
    asm volatile(
    "movl %%cr3, %%eax \n"
    "movl %%eax, %%cr3 \n"
    :
    : 
    : "%eax"
    );
}
