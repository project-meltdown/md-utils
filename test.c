#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

bool fatal_is_fatal = false;

#define __md_log_printf(format,...) printf(format,##__VA_ARGS__)

void fatal_callback() {
    if(fatal_is_fatal){
        exit(1);
    }
    
    printf("    this would exit but we wanna run the later tests too!\n");
}

#define __mdlog_fatal_callback() fatal_callback()

#define __MDLOG_TRACE // enable trace logging 
#define __MDLOG_DEBUG // enable debug logging

#include "include/md-log.h"

void test_success(char* test_name){
    log_info("Test \"%s\" succeeded!",test_name);
}

void test_fail(char* test_name){
    log_fatal("Test \"%s\" succeeded!",test_name);
}

void test_logger(){
    log_info("Testing the logger!\n\n");

    log_trace("I was here\n");
    log_debug("Yeah squashing bugs with this one\n");
    log_info ("The meaning of life is %i\n",rand()/10000);
    log_warn ("Your name is %s\n",getlogin());
    log_error("Fuck you\n");
    log_fatal("You touched Rust in your life\n");
    assert(1 + 1 == 3,"Lol\n");
}

int main(int argc, char** argv){
    srand(time(0));

    test_logger();

    fatal_is_fatal = true; //from this point onwards fatal should quit
}