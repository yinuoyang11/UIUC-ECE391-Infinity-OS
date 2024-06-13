// includes
#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include "types.h"

// structs

// filetype specific operations
typedef struct OPERATIONS {
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
} OPERATIONS;

typedef struct DIR_ENTRY {              // 64 bytes total
    struct{
        uint8_t filename[32];           // filename (32 - max length)
        uint32_t filetype;              // filetype
        uint32_t inode;                 // inode identifier
        uint8_t reserved[24];           // 24 bytes reserved

    }__attribute__((packed));
} DIR_ENTRY;

typedef struct BOOT_BLOCK {             // 64 * 64 bytes entries (4kB)
    struct{
        uint32_t dir_entries;           // number of directory entries
        uint32_t inode_entries;         // number of inode entries
        uint32_t data_blocks;           // number of data blocks
        uint8_t reserved[52];           // 52 bytes reserved
        DIR_ENTRY entries[63];          // directory entries (63 available in filesystem)
    
    }__attribute__((packed));
} BOOT_BLOCK;

typedef struct INODE {                  // 4kB total
    struct{
        uint32_t length;                // number of data blocks
        uint32_t block_ref[128 - 1];    // references to data blocks (128 - 1 = 127 available data block refs in inode)
    }__attribute__((packed));
} INODE;

typedef struct FILE_DESCRIPTOR {
    OPERATIONS* ops;                // file operations table pointer
    uint32_t inode;                 // inode number (0 - not regular file)
    uint32_t file_pos;              // where user is currently reading file
    uint32_t flags;                 // marks file as in-use (1)
    
} FILE_DESCRIPTOR;


// FUNCTIONS
// helper
void filesystem_init();
int32_t read_dentry_by_name(const uint8_t* fname, DIR_ENTRY* dentry);
int32_t read_dentry_by_index(uint32_t index, DIR_ENTRY* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
void set_fs_addr(unsigned int addr);

// file specific
int32_t open_file(const uint8_t* filename);
int32_t close_file(int32_t fd);
int32_t read_file(int32_t fd, void* buf, int32_t nbytes);
int32_t write_file(int32_t fd, const void* buf, int32_t nbytes);

// dir specific
int32_t open_dir(const uint8_t* filename);
int32_t close_dir(int32_t fd);
int32_t read_dir(int32_t fd, void* buf, int32_t nbytes);
int32_t write_dir(int32_t fd, const void* buf, int32_t nbytes);

// illegal file opes
int32_t illegal_open(const uint8_t* filename);
int32_t illegal_close(int32_t fd);
int32_t illegal_read(int32_t fd, void* buf, int32_t nbytes);
int32_t illegal_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t touch_file(uint8_t *fname, int32_t bytes);

// operations structures
extern OPERATIONS file_operations;
extern OPERATIONS dir_operations;
extern OPERATIONS rtc_operations;
extern OPERATIONS term_read;
extern OPERATIONS term_write;

#endif
