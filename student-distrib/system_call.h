#ifndef SYSTEM_CALL_H
#define SYSTEM_CALL_H

#include "filesystem.h"
#include "types.h"
#include "lib.h"
#include "x86_desc.h"
#include "page.h"
#include "scheduler.h"
#include "vga_modex.h"

#define fd_arr_size   8
#define filename_size   32
#define elf_size    4
#define eip_size    4
#define elf_magic_num 0x464c457f
#define max_process 6
#define eight_mb    0x800000
#define eight_kb    0x2000
#define user_program_start  0x08048000
#define mb_132  0x08400000
#define FIRST_PROCESS 100
#define mask_pcb    0xffffe000
#define args_size   128
#define user_vid_mem 0x8c00000 //let user video mem start from 140MB and take up 4kb
#define mode13h_vid_mem 0xa0000
#define modex_vid_size_4kb 16
#define NUM_SIGS 7

typedef struct PCB
{   
    FILE_DESCRIPTOR fd_array[fd_arr_size];
    uint8_t file_args[args_size];
        //signal_part
    uint8_t signal_pending[NUM_SIGS];
    uint8_t signal_masked[NUM_SIGS];
    void (*sig_handler_addresses[NUM_SIGS])();
    int process_id;
    int parent_pid;
    int kernel_esp;
    int kernel_ebp;
    int schedule_esp;
    int schedule_ebp;
}pcb_t;

int32_t execute(const uint8_t* command);
int32_t halt(uint8_t status);
pcb_t* get_pcb(uint32_t pid);
pcb_t* get_pcb_with_esp();
// BIG FUNCTIONS
int32_t getargs(uint8_t* buf, int32_t nbytes);
int32_t vidmap(uint8_t** screen_start);
int32_t open(const uint8_t* filename);
int32_t close(int32_t fd);
int32_t read(int32_t fd, const void* buf, int32_t nbytes);
int32_t write(int32_t fd, const void* buf, int32_t nbytes);
int32_t touch(uint8_t *fname, int32_t bytes);

#endif
