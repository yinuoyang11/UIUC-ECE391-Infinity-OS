#include "filesystem.h"
#include "lib.h"
#include "rtc.h"
#include "terminal.h"
#include "system_call.h"

unsigned int FILESYS_ADDR;
BOOT_BLOCK *fs;

// file operations struct
OPERATIONS file_operations = {
    open_file,
    close_file,
    read_file,
    write_file
};

// directory operations struct
OPERATIONS dir_operations = {
    open_dir,
    close_dir,
    read_dir,
    write_dir
};

// rtc operations struct
OPERATIONS rtc_operations = {
    rtc_open,
    rtc_close,
    rtc_read,
    rtc_write
};

// terminal read struct
OPERATIONS term_read = {
    illegal_open,
    illegal_close,
    terminal_read,
    illegal_write
};

// terminal write struct
OPERATIONS term_write = {
    illegal_open,
    illegal_close,
    illegal_read,
    terminal_write
};
/* 
 * filesystem_init
 *   DESCRIPTION: initializes the filesystem
 *   INPUTS: none
 *   OUTPUTS: None
 *   RETURN VALUE: void
 *   SIDE EFFECTS: sets the pointer to the filesystem to a global variable with
 *                 the boot block data structure
 */
void filesystem_init() {
    // initialize pointer to filesystem
    fs = (BOOT_BLOCK *)FILESYS_ADDR;
}

/* 
 * set_fs_addr
 *   DESCRIPTION: setter function to the address to the filesystem
 *   INPUTS: addr -> address to the file system
 *   OUTPUTS: None
 *   RETURN VALUE: void
 *   SIDE EFFECTS: sets the global variable of the address to the filesystem
 */
void set_fs_addr(unsigned int addr) {
    FILESYS_ADDR = addr;
}

/* 
 * read_dentry_by_name
 *   DESCRIPTION: finds the directory entry associated with a given filename
 *   INPUTS: fname - filename of wanted file
 *           dentry - pointer to dentry to recieve
 *   RETURN VALUE: 0 - success, -1 - failure
 *   SIDE EFFECTS: points given dentry pointer to found directory entry in filesystem structure
 */
int32_t read_dentry_by_name(const uint8_t* fname, DIR_ENTRY* dentry) {
    DIR_ENTRY *current;
    int8_t *char_ptr;
    int8_t *temp_ptr;
    int32_t i;
    int32_t unequal_flag = 0;

    int32_t count = (*fs).dir_entries;

    // make sure dentry count isn't 0, else fail
    if (count == 0) {
        return -1;
    }

    // move filesystem pointer to first directory entry
    current = (*fs).entries;
    char_ptr = (int8_t*)current;
    temp_ptr = (int8_t*)fname;

    // loop thorugh directory entries
    while (count > 0) {

        // test if filenames match
        for (i = 0; i < 32; i++) { // 32 - max length of filename
            if (*char_ptr != *temp_ptr) {
                unequal_flag = 1;
            }

            if ((int8_t)(*char_ptr) == (int8_t)'\0' ||
                (int8_t)(*temp_ptr) == (int8_t)'\0') {
                break;
            }

            char_ptr++;
            temp_ptr++;
        }

        if (unequal_flag == 0) {    // found dentry!
            *dentry = *current;
            return 0;               // success
        }

        unequal_flag = 0;
        count--;
        current += 1;                   // increment to next dentry (64 bytes)
        char_ptr = (int8_t*)current;
        temp_ptr = (int8_t*)fname;
    }

    // could not find dentry, fail
    return -1;
}

/* 
 * read_dentry_by_index
 *   DESCRIPTION: finds the directory entry associated with a given boot block index
 *   INPUTS: index - boot block index of wanted file
 *           dentry - pointer to dentry to recieve
 *   RETURN VALUE: 0 - success, -1 - failure
 *   SIDE EFFECTS: points given dentry pointer to found directory entry in filesystem structure
 */
int32_t read_dentry_by_index(uint32_t index, DIR_ENTRY* dentry) {
    DIR_ENTRY *current;
    int32_t i = 0;

    int32_t count = (*fs).dir_entries;

    // make sure dentry count isn't 0, else fail
    if (count == 0) {
        return -1;
    }

    // move filesystem pointer to first directory entry
    current = (*fs).entries;

    // loop thorugh directory entries
    while (i < count) {

        if (i == index) {           // found dentry!
            *dentry = *current;
            return 0;               // success
        }

        i++;
        current += 1;               // increment to next dentry (64 bytes)
    }

    // could not find dentry, fail
    return -1;
}

/* 
 * read_data
 *   DESCRIPTION: reads a file for a certain length starting a given offset
 *   INPUTS: inode - inode of wanted file
 *           offset - offset to start reading file
 *           buf - buffer to place read data into
 *           length - length of data to read from file
 *   RETURN VALUE: 0 - success, -1 - failure
 *   SIDE EFFECTS: buffer (buf) filled with file data
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    INODE *current;
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t byte_count = 0;
    uint32_t length_copy = length;
    // consider offset
    uint32_t block_offset = offset / 4096;
    uint32_t small_offset = offset % 4096;
    uint32_t small_offset_copy = small_offset;

    // find inode block from given index
    current = (INODE*)(fs + 1 + inode);

    // how much data left in file (in bytes)
    uint32_t block_count = (*current).length - offset;
    
    uint8_t *curr_block;

    if (offset >= (*current).length) {
        // read entire file
        return 0;
    }

    while(1) {
        // get block ref (offset + N + 1)
        curr_block = (uint8_t*)(fs + (*current).block_ref[i + block_offset] + (*fs).inode_entries + 1);

        // move according to offset
        while(small_offset_copy != 0) {
            // get block ref (offset + N + 1)
            curr_block++;
            small_offset_copy--;
        }

        while(j < 4096 - small_offset) {     // 4096 = number of bytes in data block
            *buf = *curr_block;
            byte_count++;
            buf++;
            curr_block++;
            j++;
            
            // file length
            block_count--;
            
            // number of bytes (argument)
            length_copy--;

            if (length_copy == 0) {
                // end of buffer length

                return byte_count;
            }
            if(block_count == 0) {
                // end of file reached
                return byte_count;
            }
        }

       // printf("length: %u\n", length_copy);

        small_offset = 0;
        j = 0;
        i++;
    }

    return 0;
}

/* 
 * write_data
 *   DESCRIPTION: writes a file for a certain length starting a given offset
 *   INPUTS: inode - inode of wanted file
 *           offset - offset to start reading file
 *           buf - buffer of text to write into file
 *           length - length of data to read from file
 *   RETURN VALUE: 0 - success, -1 - failure
 *   SIDE EFFECTS: file is adjusted
 */
int32_t write_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    INODE *current;
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t byte_count = 0;
    uint32_t length_copy = length;
    // consider offset
    uint32_t block_offset = offset / 4096;
    uint32_t small_offset = offset % 4096;
    uint32_t small_offset_copy = small_offset;

    // find inode block from given index
    current = (INODE*)(fs + 1 + inode);

    // how much data left in file (in bytes)
    uint32_t block_count = (*current).length - offset;
    
    uint8_t *curr_block;

    if (offset >= (*current).length) {
        // read entire file
        return 0;
    }

    while(1) {
        // get block ref (offset + N + 1)
        curr_block = (uint8_t*)(fs + (*current).block_ref[i + block_offset] + (*fs).inode_entries + 1);

        // move according to offset
        while(small_offset_copy != 0) {
            // get block ref (offset + N + 1)
            curr_block++;
            small_offset_copy--;
        }

        while(j < 4096 - small_offset) {     // 4096 = number of bytes in data block
            *curr_block = *buf;
            byte_count++;
            buf++;
            curr_block++;
            j++;
            
            // file length
            block_count--;
            
            // number of bytes (argument)
            length_copy--;

            if (length_copy == 0) {
                // end of buffer length

                return byte_count;
            }
            if(block_count == 0) {
                // end of file reached
                return byte_count;
            }
        }

       // printf("length: %u\n", length_copy);

        small_offset = 0;
        j = 0;
        i++;
    }

    return 0;
}

// read_dir index
int dentery_index = 0;

/* 
 * open_file
 *   DESCRIPTION: prepares given file to read
 *   INPUTS: filename - filename of file to open
 *   RETURN VALUE: 0 - success, -1 - failure
 *   SIDE EFFECTS: global dentry structure tdir filled with given file directory entry
 */
int32_t open_file(const uint8_t* filename) {
    return 0;
}

/* 
 * close_file
 *   DESCRIPTION: closes a given file
 *   INPUTS: fd - file descriptor object of file to close
 *   RETURN VALUE: 0 - success, -1 - failure
 *   SIDE EFFECTS: nothing at the moment
 */
int32_t close_file(int32_t fd) {
    pcb_t* pcb_ = get_pcb_with_esp();
    pcb_->fd_array[fd].flags = 0;

    // fd not found
    return 0;
}

/* 
 * read_file
 *   DESCRIPTION: reads a given file
 *   INPUTS: fd - file descriptor object of file to close
 *           buf - buffer to place file data
 *           nbytes - number of data byes to read from file
 *   RETURN VALUE: 0 - end of file reached, otherwise number of bytes read is returned
 *   SIDE EFFECTS: fills buffer (buf) with file data
 */
int32_t read_file(int32_t fd, void* buf, int32_t nbytes) {
    pcb_t* pcb_ = get_pcb_with_esp();
    FILE_DESCRIPTOR curr = pcb_->fd_array[fd];
    int res;
    // memset(buf, 0, nbytes);
    // fd found
    res = read_data(curr.inode, curr.file_pos, (uint8_t*) buf, nbytes);
    if (res == 0) {
        pcb_->fd_array[fd].file_pos = 0;
    } else if (res != -1) {
        // update file position
        pcb_->fd_array[fd].file_pos += res;
    }
    return res;

}

/* 
 * write_file
 *   DESCRIPTION: writes to a given file
 *   INPUTS: fd - file descriptor object of file to close
 *           buf - buffer to write file data (format: offset string)
 *           nbytes - number of data byes to write from file
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: nothing: read only filesystem
 */
int32_t write_file(int32_t fd, const void* buf, int32_t nbytes) {

    uint32_t curr_num, j;
    uint32_t offset = 0;
    uint8_t string[nbytes];
    memcpy(string, buf, nbytes);

    pcb_t* pcb_ = get_pcb_with_esp();
    FILE_DESCRIPTOR curr = pcb_->fd_array[fd];
    int res;

    // find number of places in input offset
    int i = 0;
    int places = 0;
    while(string[i] != 0x20) {              // 0x20 -> ascii space symbol
        i++;
        places++;
    }

    // calculate offset
    i = 0;
    while(i < places) {
        curr_num = ((uint32_t) string[i]) - 48;         // 48 - ascii start of decimal places
        for(j = places - i; j > 1; j--) {
            curr_num = curr_num * 10;                   // 10 - multiply by 10 to convert ascii number
                                                        //      to decimal value based on place
        }
        offset += curr_num;

        i++;
    }

    // for (i = places + 1; i < nbytes; i++) {
    //     if (i > places + 1 && string[i] == 0x6E && string[i-1] == 0x5C) {
    //         string[i] = 0x0A;
    //         string[i-1] = 0x20;
    //     }
    // }

    uint8_t* str = string;
    str += places + 1;

    INODE* currInode = (INODE*)(fs + 1 + curr.inode);

    if (offset + nbytes - places - 1 > currInode->length) {
        currInode->length += offset + nbytes - places - 1 - currInode->length;                    // + 1 because we will add a \n
    }
    
    // fd found
    res = write_data(curr.inode, offset, str, nbytes-places - 1);



    return 0;
}

/* 
 * open_dir
 *   DESCRIPTION: prepares given file to write
 *   INPUTS: filename - filename of directory to open
 *   RETURN VALUE: 0 - success, -1 - failure
 *   SIDE EFFECTS: global dentry structure tdir filled with given file directory entry
 */
int32_t open_dir(const uint8_t* filename) {
    return 0;
}

/* 
 * close_dir
 *   DESCRIPTION: closes a given directory
 *   INPUTS: fd - file descriptor object of directory to close
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: nothing
 */
int32_t close_dir(int32_t fd) {
    pcb_t* pcb_ = get_pcb_with_esp();
    pcb_->fd_array[fd].flags = 0;

    // fd not found
    return 0;
}

/* 
 * read_dir
 *   DESCRIPTION: reads a given file within the openned directory
 *   INPUTS: fd - file descriptor object of file to close
 *           buf - buffer to place file data
 *           nbytes - number of data byes maximum for buffer
 *   RETURN VALUE: 0 - success
 *   SIDE EFFECTS: fills buffer (buf) with file data (filename, filetype, size)
 */

int32_t read_dir(int32_t fd, void* buf, int32_t nbytes) {
    int res, i, copylen;
    uint8_t char_buf[nbytes];

    DIR_ENTRY tdir;

    // count number of dentrys
    uint32_t dentry_count = (*fs).dir_entries;
    if (dentery_index >= dentry_count) {
        // already read all filenames
        dentery_index = 0;
        return 0;
    }

    // find dentry of current file index
    res = read_dentry_by_index(dentery_index, &tdir);

    // copy filename into char_buf
    for (i = 0; i < strlen(((int8_t*) tdir.filename)); i++) {
        char_buf[i] = tdir.filename[i];
    }

    // assure length is not greater than 32
    copylen = strlen(((int8_t*) tdir.filename));
    if (strlen(((int8_t*) tdir.filename)) > 32) {       // 32 - max filename length
        copylen = 32;                                   // 32 - max filename length
    }

    // copy data into buffer
    memcpy(buf, char_buf, copylen);

    // increment dentry index
    dentery_index++;
    return copylen;

}

/* 
 * write_dir
 *   DESCRIPTION: writes to a given directory
 *   INPUTS: fd - file descriptor object of file to close
 *           buf - buffer to write file data
 *           nbytes - number of data byes to write from file
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: nothing: read only filesystem
 */
int32_t write_dir(int32_t fd, const void* buf, int32_t nbytes) {
    // read only filesystem
    return -1;
}

/* 
 * illigal_open
 *   DESCRIPTION: called when trying to write to stdin/out
 *   INPUTS: filename - filename
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: nothing, open not allowed
 */
int32_t illegal_open(const uint8_t* filename)
{
    // printf("illegal oepn");
    return -1;
}

/* 
 * illigal_close
 *   DESCRIPTION: called when trying to close stdin/out
 *   INPUTS: fd - fd index
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: nothing, close not allowed
 */
int32_t illegal_close(int32_t fd)
{
    // printf("illegal close");
    return -1;
}

/* 
 * illigal_read
 *   DESCRIPTION: called when trying to read stdin/out
 *   INPUTS: fd - fd index
 *           buf - buffer to read into
 *           nbytes - number of bytes to read
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: nothing, read not allowed
 */
int32_t illegal_read(int32_t fd, void* buf, int32_t nbytes)
{
    // printf("illegal read");
    return -1;
}

/* 
 * illigal_write
 *   DESCRIPTION: called when trying to write to stdin/out
 *   INPUTS: fd - fd index
 *           buf - buffer to read from
 *           nbytes - number of bytes to read
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: nothing, write not allowed
 */
int32_t illegal_write(int32_t fd, const void* buf, int32_t nbytes)
{
    // printf("illegal write");
    return -1;
}


int32_t touch_file(uint8_t *fname, int32_t bytes) {

    int i = 0;

    if (bytes > 32) {
        printf("Maximum filename size is 32\n");
    }

    // create space in the filesystem
    fs->dir_entries += 1;
    fs->inode_entries += 1;
    fs->data_blocks += 1;

    // create new inode
    INODE new_inode = {};
    new_inode.length = 0;                          // THIS IS FOR TESTING
    new_inode.block_ref[0] = fs->data_blocks - 1;

    // create new directory entry
    DIR_ENTRY new_dentry = {};
    memcpy(new_dentry.filename, fname, bytes);
    new_dentry.filetype = 2;                        // 2 - regular file filetype
    new_dentry.inode = fs->inode_entries - 1;

    // add new directory entry to filesystem
    fs->entries[fs->dir_entries - 1] = new_dentry;

    // move back data blocks by 1 block
    // memcpy((fs + fs->inode_entries + 1), (fs + fs->inode_entries), 4096 * fs->data_blocks);

    for (i = 0; i < fs->inode_entries; i++) {
        memcpy((fs + fs->inode_entries + 1 + fs->data_blocks - i), (fs + fs->inode_entries + fs->data_blocks - i), 4096);
    }

    // // insert new inode
    memcpy((fs + (fs->inode_entries)) , &new_inode, 4096);

    return 0;
}
