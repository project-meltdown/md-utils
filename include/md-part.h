/* Meltdown Partition library 0.0.1
 * 
 * This utility is for managing MBR or GPT partitions on a disk
 *
 * Copyright <year range> The Meltdown project 
 * Licensed under the BSD Zero Clause license
 *
 * Usage:
 * <put usage here>
 *
 * THIS CODE WONT WORK IF YOU DONT INCLUDE THIS IN ATLEAST ONE FILE WITH MD_PART_IMPL
 *
 */
#ifndef __MD_PART_H__
#define __MD_PART_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

//==== constraints ====

#if !defined(__md_part_read)
    #error "Constraint failed: function `bool __md_part_read(void* data,size_t offset, void* buffer, size_t num_bytes)` undeclared"
    #warning "It has to be a macro thanks to how define() works"
#endif

#if !defined(__md_part_write)
    #error "Constraint failed: function `bool __md_part_write(void* data,size_t offset, void* buffer, size_t num_bytes)` undeclared"
    #warning "It has to be a macro thanks to how define() works"
#endif

#if !defined(__md_part_alloc)
    #error "Constraint failed: function `void* __md_part_alloc()` undeclared"
    #warning "It has to be a macro thanks to how define() works"
#endif

#if !defined(__md_part_free)
    #error "Constraint failed: function `bool __md_part_free(void* allocation)` undeclared"
    #warning "It has to be a macro thanks to how define() works"
#endif

#if !defined(__md_part_memset)
    #error "Constraint failed: function `void* __md_part_memset( void * ptr, int value, size_t num )` undeclared"
    #warning "It has to be a macro thanks to how define() works"
#endif

//==== errors ====

//This error is for when the disk driver fails to read or write
#define MPE_DISKOP_FAIL 0xFFFF

//This error is for when the disk has an invalid signature
#define MPE_BAD_SIG 0xFFFE

//==== structs ====

//structures meant for external use
typedef struct __md_part {
    uint8_t id; //TODO: GUID if its GPT

    uint8_t type;

    uint32_t offset; 
    uint32_t size;

    bool bootable;

    struct __md_part* next;
} md_part_t;

typedef struct {
    void* private_data;
    size_t size;
    size_t block_size;
    size_t num_parts;

    //these partitions should be 1/4 of a page in size so every 4th one is a new allocation
    md_part_t* parts;
} md_disk_t;

//MBR structures

typedef struct {
    uint8_t head;
    //bits 6-7 are bits 8-9 of the cylinder, the rest is the sector
    uint8_t c_s;
    uint8_t c;
} __mbr_chs_addr;

typedef struct {
    uint8_t status;
    __mbr_chs_addr first_addr;
    uint8_t part_type;
    __mbr_chs_addr last_addr;
    uint32_t first_addr_lba;
    uint32_t num_blocks;
} __mbr_part;

//==== macros ====

//MBR stuff
#define MBR_SIG_OFFSET      0x1FE
#define MBR_SIG_BYTE1       0x55
#define MBR_SIG_BYTE2       0xAA
#define MBR_PART0_OFFSET    0x01BE
#define MBR_STATUS_INACTIVE 0x00
#define MBR_STATUS_BOOTABLE 0x80
#define MBR_PTYPE_EMPTY     0x00

//==== functions ====

uint16_t mdpart_explore_disk(md_disk_t* disk);

#ifdef MD_PART_IMPL

uint16_t mdpart_explore_disk(md_disk_t* disk){

    //verify boot signature
    uint8_t buf[2];
    if(!__md_part_read(disk->private_data, MBR_SIG_OFFSET, buf, 2)){
        return MPE_DISKOP_FAIL;
    }

    if (buf[0] != MBR_SIG_BYTE1 && buf[1] != MBR_SIG_BYTE2){
        return MPE_BAD_SIG;
    }

    if(disk->parts == nullptr){
        disk->parts = __md_part_alloc();
        __md_part_memset(disk->parts,0,4096);
    }

    //get primary partitions
    __mbr_part part;
    md_part_t* mpart = disk->parts;
    for(uint8_t i = 0; i < 4;i++){
        if(!__md_part_read(disk->private_data, MBR_PART0_OFFSET+i*sizeof(__mbr_part), &part, sizeof(__mbr_part))){
            return MPE_DISKOP_FAIL;
        }

        if((part.status != MBR_STATUS_INACTIVE && part.status != MBR_STATUS_BOOTABLE) || part.part_type == MBR_PTYPE_EMPTY || part.first_addr_lba == 0){
            mpart = (md_part_t*)(((char*)mpart) - 1024); //TODO: see how well they fit
            mpart->next = NULL;
            break;
        }

        mpart->id = i;
        mpart->type = part.part_type;
        mpart->offset = part.first_addr_lba * disk->block_size; //TODO: convert chs if LBA not available 
        mpart->size = part.num_blocks * disk->block_size;
        mpart->bootable = (part.status == MBR_STATUS_BOOTABLE);

        if(i < 3){
            mpart->next = (md_part_t*)(((char*)mpart) + 1024); //TODO: see how well they fit
            mpart = mpart->next;
        } else {
            mpart->next = NULL;
        }

    }

    //TODO: extended and GPT partitions


    return 0;
}

#endif // MD_PART_IMPL

#endif // __MD_PART_H__