#ifndef KLOG_H
#define KLOG_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

// Log levels (higher number = more important)
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO  = 1,
    LOG_WARN  = 2,
    LOG_ERROR = 3,
    LOG_PANIC = 4
} log_level_t;

// Log colors for different levels
typedef enum {
    LOG_COLOR_DEBUG = 8,   // Dark grey
    LOG_COLOR_INFO  = 7,   // Light grey  
    LOG_COLOR_WARN  = 14,  // Yellow
    LOG_COLOR_ERROR = 12,  // Light red
    LOG_COLOR_PANIC = 15   // White on red background
} log_color_t;

void klog_init(void);

void klog_set_level(log_level_t level);

void klog(log_level_t level, const char* subsystem, const char* format, ...);

#define KDEBUG(sys, fmt, ...) klog(LOG_DEBUG, sys, fmt, ##__VA_ARGS__)
#define KINFO(sys, fmt, ...)  klog(LOG_INFO,  sys, fmt, ##__VA_ARGS__)  
#define KWARN(sys, fmt, ...)  klog(LOG_WARN,  sys, fmt, ##__VA_ARGS__)
#define KERROR(sys, fmt, ...) klog(LOG_ERROR, sys, fmt, ##__VA_ARGS__)
#define KPANIC(sys, fmt, ...) klog(LOG_PANIC, sys, fmt, ##__VA_ARGS__)

void kernel_panic(const char* message);

void kprintf(const char* format, ...);
void kvprintf(const char* format, va_list args);

size_t kstrlen(const char* str);
void kstrcpy(char* dest, const char* src);
void kmemset(void* ptr, int value, size_t num);
void kitoa(int value, char* str, int base);

#endif
