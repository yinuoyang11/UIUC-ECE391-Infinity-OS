#include "system_call.h"

static int process_arr[max_process] = {0, 0, 0, 0, 0, 0};

/*
 * execute
 *   DESCRIPTION: Executes a given command by loading and running the corresponding executable file.
 *   INPUTS: command - The command to execute.
 *   OUTPUTS: None
 *   RETURN VALUE: Returns 0 on success, -1 on failure.
 *   SIDE EFFECTS: Modifies process data, sets up program paging, and executes the loaded program.
 */
int32_t execute(const uint8_t* command)
{
    cli();
    uint8_t filename[filename_size];
    uint32_t inode_;
    uint32_t pid_;
    uint8_t elf_buf[elf_size];
    uint8_t eip_buf[eip_size];
    uint8_t args_buf[args_size];
    DIR_ENTRY dentry;
    memset(args_buf, 0, args_size);
    memset(filename, 0, filename_size);
    int i = 0;
    int j;
    // filename can't be started with spaces
    while (command[i] != ' ' && command[i] != '\0'){    
        filename[i] = command[i];
        i ++;
    }
    if (command[i] != '\0'){
        i ++;
        for (j = i ; j < strlen((int8_t*)command) ; j++){
            args_buf[j-i] = command[j];
        }
    }
    // find the directory entry based on the filename
    if (read_dentry_by_name(filename, &dentry) == -1)   return -1;
    inode_ = dentry.inode;
    // read the elf magic number and make sure the file is executeble
    if (read_data(inode_, 0, elf_buf, elf_size) != elf_size)  return -1;
    if (*(uint32_t*)elf_buf != elf_magic_num)   return -1;  // check validity
    if (read_data(inode_, 24, eip_buf, eip_size) != eip_size) return -1;

    // allocate a process id for the new process
    for (pid_ = 0 ; pid_ < max_process ; pid_ ++){
        if (process_arr[pid_] == 0){
            process_arr[pid_] = 1;
            break;
        }
        if (pid_ == max_process - 1){
            return -1;
        }
    }


    // setup_program_paging
    page_user_program(pid_);
    pcb_t* pcb_ = get_pcb(pid_);
    // store esp and ebp values
    uint32_t esp_,ebp_;
    asm volatile("mov %%esp, %0" : "=r"(esp_));
    asm volatile("mov %%ebp, %0" : "=r"(ebp_));
    pcb_->kernel_esp = esp_;
    pcb_->kernel_ebp = ebp_;
    // store tss
    tss.esp0 = (eight_mb - eight_kb * pid_ - 4);
    tss.ss0 = KERNEL_DS;
    if (read_data(inode_, 0, (uint8_t*) user_program_start, mb_132 - user_program_start) == -1)    return -1;
    if (mode13_flag){
        sti();
        asm volatile(
        "pushl %0   \n\
        pushl %1    \n\
        pushfl      \n\
        pushl %2    \n\
        pushl %3    \n\
        iret        \n\
        "
        : 
        : "r" (USER_DS), "r" (mb_132 - sizeof(uint32_t)), "r" (USER_CS), "r" (*(uint32_t*)eip_buf)
        : "memory"
        );
    }
    // enable stdin and stdout
    for (i = 0 ; i < 8 ; i ++){
        pcb_->fd_array[i].ops = 0;
        pcb_->fd_array[i].inode = 0;
        pcb_->fd_array[i].flags = 0;
        pcb_->fd_array[i].file_pos = 0;
    }
    pcb_->fd_array[0].ops = &term_read;
    pcb_->fd_array[0].flags = (uint32_t) 1;
    pcb_->fd_array[1].ops = &term_write;
    pcb_->fd_array[1].flags = (uint32_t) 1;

    // store pcb
    memset(pcb_->file_args, 0, args_size);
    memcpy(pcb_->file_args, args_buf, args_size);
    pcb_->process_id = pid_;
    if(pcb_->process_id == 0 || pcb_->process_id == 1 || pcb_->process_id == 2){
        pcb_->parent_pid = FIRST_PROCESS;
    }
    else{
        pcb_->parent_pid = (eight_mb - esp_) / eight_kb; 
    }
    // initialize the process_arr if the shell is first called
    for (i = 0 ; i < 3 ; i++){
        if (running_process[i] == NOT_INITIALIZED){
            running_process[i] = pid_;
            break;
        }
        else if (running_process[i] == pcb_->parent_pid){
            running_process[i] = pid_;
            break;
        }
    }

    //signal_part
    for(i = 0; i < NUM_SIGS; i++){
        pcb_->signal_pending[i] = 0;
        pcb_->signal_masked[i] = 0;
        pcb_->sig_handler_addresses[i] = NULL; //init to NULL, if user sets handler, it would point to user defined handler, else it should be default

    }
    // read the content of the file from disk to memory
    sti();
    // set up stack for context switch and iret to user program
    asm volatile(
        "pushl %0   \n\
        pushl %1    \n\
        pushfl      \n\
        pushl %2    \n\
        pushl %3    \n\
        iret        \n\
        "
    : 
    : "r" (USER_DS), "r" (mb_132 - sizeof(uint32_t)), "r" (USER_CS), "r" (*(uint32_t*)eip_buf)
    : "memory"
    );

    return 0;
}

/*
 * halt
 *   DESCRIPTION: Halts the currently executing process and returns control to its parent process.
 *   INPUTS: status - The status to be returned to the parent process.
 *   OUTPUTS: None
 *   RETURN VALUE: Should not return. If an exception occurs, returns 256.
 *   SIDE EFFECTS: Closes relevant file descriptors and modifies process and TSS data.
 */
int32_t halt(uint8_t status)
{
    uint32_t ret = (uint32_t)status;
    int i;
/*handle an exception*/
    if (status == 255){
        ret = 256;
    }
    pcb_t* cur_process = get_pcb_with_esp();
    uint32_t par_pid = cur_process->parent_pid;


/*mark as unused*/
    process_arr[cur_process->process_id] = 0;
    if (mode13_flag){
        tss.ss0 = KERNEL_DS;
        tss.esp0 = 0x800000;
        asm volatile("movl %0, %%ebp \n\
                  movl %1, %%eax \n\
                  leave          \n\
                  ret            \n"
                : 
                : "r" (cur_process->kernel_ebp),"r" (ret)
                : "eax", "ebp", "esp");      
        return ret; 
    }
/*restore parent data*/
    if ((cur_process->process_id == 0 || cur_process->process_id == 1 || cur_process->process_id == 2)){
        printf("You can't close the base shell\n");
        execute((uint8_t*)"shell");
    }
    running_process[scheduled_terminal] = par_pid;
/*restore parent paging*/
    page_user_program(par_pid);

/*modify tss*/
    tss.ss0 = KERNEL_DS;
    tss.esp0 = eight_mb - par_pid * eight_kb - 4;

/*close relevant fds*/
    for (i = 0; i < 8; i++)
        if (cur_process->fd_array[i].flags){
            cur_process->fd_array[i].flags = 0;
            cur_process->fd_array[i].ops->close(i);
        }

/*go back to return of execute*/
    asm volatile("movl %0, %%ebp \n\
                  movl %1, %%eax \n\
                  leave          \n\
                  ret            \n"
                : 
                : "r" (cur_process->kernel_ebp),"r" (ret)
                : "eax", "ebp", "esp");      
   return ret; 
}

/*
 * get_pcb
 *   DESCRIPTION: Retrieves the Process Control Block (PCB) for a given process ID.
 *   INPUTS: pid - Process ID of the desired process.
 *   OUTPUTS: None
 *   RETURN VALUE: Pointer to the PCB of the specified process.
 *   SIDE EFFECTS: None
 */

pcb_t* get_pcb(uint32_t pid)
{
    return (pcb_t*) (eight_mb - eight_kb * (pid + 1));
}

/*
 * get_pcb_with_esp
 *   DESCRIPTION: Retrieves the Process Control Block (PCB) using the current stack pointer.
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: Pointer to the PCB of the current process.
 *   SIDE EFFECTS: None
 */
pcb_t* get_pcb_with_esp()
{
    uint32_t esp_;
    asm volatile("mov %%esp, %0" : "=r"(esp_));
    return (pcb_t*)(esp_ & mask_pcb);
}

/* 
 * open
 *   DESCRIPTION: opens a designted file
 *   INPUTS: filename - filename of file to open
 *   RETURN VALUE: 0 - success, -1 - failure
 *   SIDE EFFECTS: places associated file directory into file array
 */
int32_t open(const uint8_t* filename) {

    int res = 0;
    int i = 0;
    // get current pcb
    pcb_t* pcb_ = get_pcb_with_esp();
    // loop through file array
    for (i = 2; i < 8; i++) {                       // 2 - assures cannot open stdin/stdout
                                                    // 8 - max number of files open
        // check if space in file array
        if (pcb_->fd_array[i].flags == 0) {
            break;
            // thus i = free index in file array
        }
        if (i == 7) {                               // 7 - last index of filearray (8 files max)
            // no room to open file
            return -1;
        }
    }

    // find dentry of file
    DIR_ENTRY tdir;
    res = read_dentry_by_name(filename, &tdir);

    if (res == -1) {
        // file does not exsist
        return -1;
    }
    
    // populate FILE_DISCRIPTOR         
    pcb_->fd_array[i].inode = tdir.inode;
    pcb_->fd_array[i].file_pos = 0;
    pcb_->fd_array[i].flags = 1;                    // flags - 1 to designate file as open

    // populates operations in FILE_DISCRIPTOR
    if (tdir.filetype == 2) {                       // 2 - regular filetype
        pcb_->fd_array[i].ops = &file_operations;
        (pcb_->fd_array[i].ops)->open(filename);
    } else if (tdir.filetype == 1) {                // 1 - directory filetype
        pcb_->fd_array[i].ops = &dir_operations;
        (pcb_->fd_array[i].ops)->open(filename);
    }
    else if (tdir.filetype == 0) {                  // 0 - rtc filetype
        pcb_->fd_array[i].ops = &rtc_operations;
        (pcb_->fd_array[i].ops)->open(filename);
    }

    return i;
}

/* 
 * close
 *   DESCRIPTION: closes an open file
 *   INPUTS: fd - fd index of file to close
 *   RETURN VALUE: 0 - success, -1 - failure
 *   SIDE EFFECTS: removes file from file array designating it as closed
 */
int32_t close(int32_t fd) {
    pcb_t* pcb_ = get_pcb_with_esp();
    if (fd > 1 && fd < 8) {                         // 8 - max number of files open
        if (pcb_->fd_array[fd].flags == 0) {
            // file was not opened
            return -1;
        }
        return (pcb_->fd_array[fd].ops)->close(fd);
    }

    // else does not exsist or not allowed
    return -1;
}

/* 
 * read
 *   DESCRIPTION: reads open file
 *   INPUTS: fd - fd index to read into
 *           buf - buffer to write from
 *           nbytes - count of bytes to transfer
 *   RETURN VALUE: 0 - success, -1 - failure
 *   SIDE EFFECTS: reads nbytes of data from file designated by fd into buf
 */
int32_t read(int32_t fd, const void* buf, int32_t nbytes) {
    pcb_t* pcb_ = get_pcb_with_esp();
    if (fd >= 0 && fd < 8) {                                            // 8 - max number of files open
        if (pcb_->fd_array[fd].flags == 0) {
            // file was not opened
            return -1;
        }
        return (pcb_->fd_array[fd].ops)->read(fd, (void*) buf, nbytes);
    }

    // else does not exsist or not allowed
    return -1;
}

/* 
 * write
 *   DESCRIPTION: writes to open file
 *   INPUTS: fd - fd index to write to
 *           buf - buffer to write from
 *           nbytes - count of bytes to transfer
 *   RETURN VALUE: 0 - success, -1 - failure
 *   SIDE EFFECTS: writes nbytes of content of buf into file designated by fd
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes) {
    pcb_t* pcb_ = get_pcb_with_esp();
    if (fd >= 0 && fd < 8) {                                        // 8 - maximum number of open files allowed
        if (pcb_->fd_array[fd].flags == 0) {
            // file was not opened
            return -1;
        }
        return (pcb_->fd_array[fd].ops)->write(fd, buf, nbytes);
    }

    // else does not exsist or not allowed
    return -1;
}

int32_t touch(uint8_t *fname, int32_t bytes) {
    // call touch_file in filesystem
    return touch_file(fname, bytes);
}

/*
 * getargs
 *   DESCRIPTION: Retrieves the characters passed to the currently executing program.
 *   INPUTS: buf - Buffer to store the characters.
 *           nbytes - number of bytes to read.
 *   OUTPUTS: None
 *   RETURN VALUE: Returns 0 on success, -1 on failure.
 *   SIDE EFFECTS: Copies characters to the provided buffer.
 */
int32_t getargs(uint8_t* buf, int32_t nbytes){
    pcb_t* pcb_ = get_pcb_with_esp();
    if (pcb_ == NULL || buf == NULL || pcb_->file_args[0] == '\0')    return -1;
    memcpy(buf, pcb_->file_args, nbytes);
    return 0;
}


/*
 * vidmap
 *   DESCRIPTION: Maps video memory into user space at a specified address.
 *   INPUTS: screen_start - Pointer to store the mapped address.
 *   OUTPUTS: None
 *   RETURN VALUE: Returns 0 on success, -1 on failure.
 *   SIDE EFFECTS: Modifies page directory and page table entries to map video memory. Flushes tlb. 
 */
int32_t vidmap(uint8_t** screen_start){
    //screen_start should point to an address in loaded program (128-132MB)
    //range check

    if ((screen_start >= (uint8_t**)(user_program_start + four_mb)) || (screen_start < (uint8_t**)user_program_start) || screen_start == NULL)
        return -1; 
    
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
    *screen_start = (uint8_t*) user_vid_mem; //point to newly assigned virtual address
    flush_tlb(); 
    return 0;
}

int32_t song_main(){
    Note melody[] = {
{329, 300}, {0, 100}, {392, 300}, {0, 100}, {587, 300}, {0, 100}, {523, 300}, 
{0, 100}, {587, 300}, {0, 100}, {523, 300}, {0, 100}, {392, 300}, {0, 100}, {523, 300},
{0, 100}, {329, 300}, {0, 100}, {392, 300}, {0, 100}, {587, 300}, {0, 100}, {523, 300}, 
{0, 100}, {587, 300}, {0, 100}, {523, 300}, {0, 100}, {392, 300}, {0, 100}, {523, 300}, 
{0, 100}, {300, 293}, {0, 100}, {392, 300}, {0, 100}, {587, 300}, {0, 100}, {523, 300}, 
{0, 100}, {587, 300}, {0, 100}, {523, 300}, {0, 100}, {392, 300}, {0, 100}, {523, 300}, 
{0, 100}, {300, 293}, {0, 100}, {392, 300}, {0, 100}, {587, 300}, {0, 100}, {523, 300}, 
{0, 100}, {587, 300}, {0, 100}, {523, 300}, {0, 100}, {392, 300}, {0, 100}, {523, 300},
{0, 100}, {329, 300}, {0, 100}, {440, 300}, {0, 100}, {587, 300}, {0, 100}, {523, 300},
{0, 100}, {587, 300}, {0, 100}, {523, 300}, {0, 100}, {440, 300}, {0, 100}, {523, 300},
{0, 100}, {329, 300}, {0, 100}, {440, 300}, {0, 100}, {587, 300}, {0, 100}, {523, 300},
{0, 100}, {587, 300}, {0, 100}, {523, 300}, {0, 100}, {440, 300}, {0, 100}, {523, 300},
{0, 100}, {300, 349}, {0, 100}, {392, 300}, {0, 100}, {587, 300}, {0, 100}, {523, 300}, 
{0, 100}, {587, 300}, {0, 100}, {523, 300}, {0, 100}, {392, 300}, {0, 100}, {523, 300}, 
{0, 100}, {300, 349}, {0, 100}, {392, 300}, {0, 100}, {587, 300}, {0, 100}, {523, 300}, 
{0, 100}, {587, 300}, {0, 100}, {523, 300}, {0, 100}, {392, 300}, {0, 100}, {523, 300}, {0, 100}
};

    int num_notes = sizeof(melody) / sizeof(Note);
    // Play the melody
    int i;
    // int32_t rtc_freq = 2;
    // rtc_fd = rtc_open((uint8_t*)"rtc");
    // ret = rtc_write(0,&rtc_freq,4);
    for (i = 0; i < num_notes; i++) {
        play_note_with_rtc(melody[i].frequency, melody[i].duration);
    //     for (j = 0; j < 1000; j++) {
    //         ret = rtc_read(0,&garbage,4); // Wait for an RTC interrupt (synchronous wait)
    // }
    // ret = rtc_close(0);
	// turn_off_sound();
	// music_ticks = 0;
	// while (music_ticks < 256 ){};
    }
	return 0;
}

int32_t show_time(){
    int second_,minute_,hour_;
    rtc_time();
    second_ = second;
    minute_ = minute;
    hour_ = hour - 5;
    printf("Current Time: %d:%d:%d\n", hour_, minute_, second_);    
}
