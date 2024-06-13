#ifndef PAGE_H
#define PAGE_H
#include "types.h"
#include "lib.h"

#define KERNEL_PAGE 4*1024*1024
#define USER_SPACE_START 0x800000
#define four_mb 0x400000
#define four_kb 0x1000
#define USER_VIRTUAL_MEM 0x8000000
#define TERM1_VID 0xBA000
#define TERM2_VID 0xBB000
#define TERM3_VID 0xBC000

typedef union PDE_MB {
    uint32_t content; 
    struct{
        uint32_t p : 1; //present
        uint32_t rw : 1;
        uint32_t us : 1;
        uint32_t pwt : 1; //write-through
        uint32_t pcd : 1; //cache disable
        uint32_t a : 1; //accessed
        uint32_t d : 1; //dirty
        uint32_t ps : 1; //page size
        uint32_t g : 1; 
        uint32_t avl : 3; //available 
        uint32_t pat : 1; //page attribute table.
        uint32_t addr_h : 8; // bits 39-32 of addr
        uint32_t rsvd : 1; 
        uint32_t addr_l : 10; //bits 31-22 of addr
    }__attribute__((packed));
}PDE_MB;

typedef union PDE_KB {
    uint32_t content;
    struct{
        uint32_t p : 1; //present
        uint32_t rw : 1;
        uint32_t us : 1; //user/supervisor
        uint32_t pwt : 1; //write-through
        uint32_t pcd : 1; //cache disable
        uint32_t a : 1; //accessed
        uint32_t avl : 1;
        uint32_t ps : 1;
        uint32_t avl2 : 4; //available 
        uint32_t addr : 20; //bits 31-22 of addr
    }__attribute__((packed));
}PDE_KB;

typedef union PDE {
    PDE_MB MB;
    PDE_KB KB;
} PDE;

typedef union PTE {
    uint32_t content;
    struct {
        uint32_t p : 1; //present
        uint32_t rw : 1;
        uint32_t us : 1;
        uint32_t pwt : 1; //write-through
        uint32_t pcd : 1; //cache disable
        uint32_t a : 1; //accessed
        uint32_t d : 1; //dirty
        uint32_t pat : 1; //page attribute table
        uint32_t g : 1;
        uint32_t avl : 3;
        uint32_t addr: 20; //bits 31-12 of address
    }__attribute__((packed));
}PTE;

//create arrays for PDE and PTE
PDE page_dir[1024]__attribute__((aligned(4096))); //1024 entries in page directory
PTE page_table[1024]__attribute__((aligned(4096))); //1024 entries in page table
PTE page_table_2[1024]__attribute__((aligned(4096)));
PTE mode13h_page_table[1024]__attribute__((aligned(4096)));

void page_init();
void page_user_program(uint32_t pid);
void sched_update_vid_paging(uint8_t scheduled_term, uint8_t viewed_term);
void sched_update_user_vid_paging(uint8_t scheduled_term, uint8_t viewed_term);
void map_vid_to_vid(); 
void map_mode13h();
void flush_tlb();
#endif
