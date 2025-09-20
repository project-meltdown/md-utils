#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

bool fatal_is_fatal = false;

#define __md_log_printf(format,...) printf(format,##__VA_ARGS__)

void fatal_callback() {
    if(fatal_is_fatal){
        exit(1);
    }
    
    printf("(fatal callback triggered)\n");
}

#define __mdlog_fatal_callback() fatal_callback()

#define __MDLOG_TRACE // enable trace logging 
#define __MDLOG_DEBUG // enable debug logging

#include "include/md-log.h"

#define MD_PART_IMPL

//writes to a disk
bool _read(void* data,size_t offset, void* buffer, size_t num_bytes){
    int fd = *((int*)data);

    ssize_t res = pread(fd,buffer,num_bytes,offset);

    if(res == -1){
        return false;
    }

    return true;
}

//writes to a disk
bool _write(void* data,size_t offset, void* buffer, size_t num_bytes){
    int fd = *((int*)data);

    ssize_t res = pwrite(fd,buffer,num_bytes,offset);

    if(res == -1){
        return false;
    }

    return true;
}

//allocates pages
void* _alloc(){
    return malloc(4096);
}

//frees pages
void _free(void* ptr){
    free(ptr);
}

#define __md_part_read(...) _read(__VA_ARGS__)
#define __md_part_write(...) _write(__VA_ARGS__)
#define __md_part_alloc() _alloc()
#define __md_part_free(...) _free(__VA_ARGS__)
#define __md_part_memset(...) memset(__VA_ARGS__)

#include "include/md-part.h"

#define test_success(test_name) log_info("Test \"%s\" succeeded!\n",test_name);
#define test_fail(test_name,msg,...) log_fatal("Test \"%s\" failed: " msg,test_name,##__VA_ARGS__);

void test_logger(){
    log_info("Testing md-log!\n\n");

    log_trace("I was here\n");
    log_debug("Yeah squashing bugs with this one\n");
    log_info ("The meaning of life is %i\n",rand()/10000);
    log_warn ("Your name is %s\n",getlogin());
    log_error("Fuck you\n");
    log_fatal("You touched Rust in your life\n");
    assert(1 + 1 == 3,"Lol\n");
}

void setup_part(md_disk_t* disk){
    FILE* link = fopen("testimgs/mbr.img", "r");
    
    if(link == NULL){
        log_fatal("Setup for testing md-part failed: failed to open \"testimgs/mbr.img\"\n");
    }

    int* filedes = malloc(sizeof(int));
    *filedes = fileno(link);

    fseek(link,0,SEEK_END);
    size_t size = ftell(link);
    fseek(link,0,SEEK_SET);

    disk->private_data = filedes;
    disk->size = size;
    disk->block_size = 512;
}
void test_part(md_disk_t* disk){
    log_info("Testing md-part!\n\n");

    //disk exploration test (MBR)
    uint16_t status = mdpart_explore_disk(disk);

    if(status == 0){
        test_success("disk exploration(MBR)");
    } else{
        test_fail("disk exploration(MBR)","mpart_explore_disk returned 0x%x\n",status);
    }

    md_part_t* part = disk->parts;
    while(part != NULL){
        log_info("Partition %i is %s\n",part->id,part->bootable ? "bootable" : "inactive");
        log_info("Partition %i type: 0x%x\n",part->id,part->type);
        log_info("Partition %i offset: %u bytes\n",part->id,part->offset);
        log_info("Partition %i size: %u bytes\n",part->id,part->size);

        part = part->next;
    }

}

int main(int argc, char** argv){
    srand(time(0));

    test_logger();

    fatal_is_fatal = true; //from this point onwards fatal should quit

    md_disk_t disk = {0};

    printf("\n");
    setup_part(&disk);
    test_part(&disk);

}