/* Meltdown Logging uitlity) 0.0.1
 * 
 * This utility provides logging and assertion features for C programs
 *
 * Copyright 2025 The Meltdown project 
 * Licensed under the BSD Zero Clause license
 *
 * Usage:
 * Define `__md_log_printf(format,...)` and optionally `__mdlog_fatal_callback()`
 * and then you can use the logging functions bellow
 *
 */
#ifndef __LOGGING_H__
#define __LOGGING_H__

#if defined(__mdlog_fatal_callback)
    #define FATAL_CALLBACK __mdlog_fatal_callback()
#else
    #define FATAL_CALLBACK 
#endif

#if !defined(__MDLOG_LOG_PATTERN)
    #if !defined(__md_log_printf)
        #error "Undefined function `__md_log_printf(format,...)` required for construction of the default log pattern"
    #endif

    #define __MDLOG_LOG_PATTERN(level,msg,...) __md_log_printf("[" level "] at " __FILE__":%i: " msg,__LINE__, ##__VA_ARGS__)
#endif

#if !defined(__MDLOG_LEVELSTR_TRACE)
    #define __MDLOG_LEVELSTR_TRACE "TRACE"
#endif

#if !defined(__MDLOG_LEVELSTR_DEBUG)
    #define __MDLOG_LEVELSTR_DEBUG "DEBUG"
#endif

#if !defined(__MDLOG_LEVELSTR_INFO)
    #define __MDLOG_LEVELSTR_INFO " INFO"
#endif

#if !defined(__MDLOG_LEVELSTR_WARN)
    #define __MDLOG_LEVELSTR_WARN " WARN"
#endif

#if !defined(__MDLOG_LEVELSTR_ERROR)
    #define __MDLOG_LEVELSTR_ERROR "ERROR"
#endif

#if !defined(__MDLOG_LEVELSTR_FATAL)
    #define __MDLOG_LEVELSTR_FATAL "FATAL"
#endif

#if defined(__MDLOG_TRACE)
    #define log_trace(msg,...) __MDLOG_LOG_PATTERN(__MDLOG_LEVELSTR_TRACE,msg,##__VA_ARGS__)
#endif

#if defined(__MDLOG_DEBUG)
    #define log_debug(msg,...) __MDLOG_LOG_PATTERN(__MDLOG_LEVELSTR_DEBUG,msg,##__VA_ARGS__)
#endif

#define log_info(msg,...)  __MDLOG_LOG_PATTERN(__MDLOG_LEVELSTR_INFO ,msg,##__VA_ARGS__)
#define log_warn(msg,...)  __MDLOG_LOG_PATTERN(__MDLOG_LEVELSTR_WARN ,msg,##__VA_ARGS__)
#define log_error(msg,...) __MDLOG_LOG_PATTERN(__MDLOG_LEVELSTR_ERROR,msg,##__VA_ARGS__)
#define log_fatal(msg,...) __MDLOG_LOG_PATTERN(__MDLOG_LEVELSTR_FATAL,msg,##__VA_ARGS__);FATAL_CALLBACK // this will inserta fatal callback

#define assert(cond,msg,...) if (!(cond)) { log_fatal("assertion failed: " msg,##__VA_ARGS__); }

#endif // __LOGGING_H__