#include "klog.h"
#include "vga.h"

static log_level_t min_log_level = LOG_DEBUG;

static const char* log_level_names[] = {
    "DEBUG", "INFO ", "WARN ", "ERROR", "PANIC"
};

static const uint8_t log_level_colors[] = {
    8,  // DEBUG - Dark grey
    7,  // INFO  - Light grey
    14, // WARN  - Yellow  
    12, // ERROR - Light red
    15  // PANIC - White
};

void klog_init(void) {
    KINFO("KLOG", "Kernel logging system initialized");
}

void klog_set_level(log_level_t level) {
    min_log_level = level;
    KINFO("KLOG", "Log level set to %s", log_level_names[level]);
}

void klog(log_level_t level, const char* subsystem, const char* format, ...) {
    if (level < min_log_level) {
        return;
    }
    
    if (level == LOG_PANIC) {
        terminal_setcolor(vga_entry_color(15, 4));
    } else {
        terminal_setcolor(vga_entry_color(log_level_colors[level], 0));
    }
    
    terminal_writestring("[");
    terminal_writestring(log_level_names[level]);
    terminal_writestring("] ");
    terminal_writestring(subsystem);
    terminal_writestring(": ");
    
    va_list args;
    va_start(args, format);
    kvprintf(format, args);
    va_end(args);
    
    terminal_putchar('\n');

    terminal_setcolor(vga_entry_color(7, 0));
    
    if (level == LOG_PANIC) {
        kernel_panic("PANIC log message triggered");
    }
}

void kernel_panic(const char* message) {
    __asm__ volatile ("cli");
    
    terminal_setcolor(vga_entry_color(15, 4));
    terminal_writestring("\n\n*** KERNEL PANIC ***\n");
    terminal_writestring("System halted due to critical error:\n");
    terminal_writestring(message);
    terminal_writestring("\n\nSystem must be restarted.\n");
    
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void kprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    kvprintf(format, args);
    va_end(args);
}

void kvprintf(const char* format, va_list args) {
    char buffer[512];
    char* buf_ptr = buffer;
    const char* fmt_ptr = format;
    
    while (*fmt_ptr) {
        if (*fmt_ptr == '%') {
            fmt_ptr++; // Skip the %
            
            switch (*fmt_ptr) {
                case 's': { // String
                    char* str = va_arg(args, char*);
                    while (*str) {
                        *buf_ptr++ = *str++;
                    }
                    break;
                }
                case 'd': { // Decimal integer
                    int value = va_arg(args, int);
                    char num_str[32];
                    kitoa(value, num_str, 10);
                    char* num_ptr = num_str;
                    while (*num_ptr) {
                        *buf_ptr++ = *num_ptr++;
                    }
                    break;
                }
                case 'x': { // Hexadecimal
                    int value = va_arg(args, int);
                    char num_str[32];
                    kitoa(value, num_str, 16);
                    char* num_ptr = num_str;
                    while (*num_ptr) {
                        *buf_ptr++ = *num_ptr++;
                    }
                    break;
                }
                case 'c': { // Character
                    char c = (char)va_arg(args, int);
                    *buf_ptr++ = c;
                    break;
                }
                case '%': { // Literal %
                    *buf_ptr++ = '%';
                    break;
                }
                default:
                    // Unknown format specifier
                    *buf_ptr++ = '%';
                    *buf_ptr++ = *fmt_ptr;
                    break;
            }
        } else {
            *buf_ptr++ = *fmt_ptr;
        }
        fmt_ptr++;
    }
    
    *buf_ptr = '\0';
    terminal_writestring(buffer);
}

// String utilities
size_t kstrlen(const char* str) {
    size_t len = 0;
    while (str[len]) {
        len++;
    }
    return len;
}

void kstrcpy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void kmemset(void* ptr, int value, size_t num) {
    unsigned char* p = (unsigned char*)ptr;
    while (num--) {
        *p++ = (unsigned char)value;
    }
}

void kitoa(int value, char* str, int base) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    int tmp_value;
    
    if (value < 0 && base == 10) {
        *ptr++ = '-';
        value = -value;
        ptr1++;
    }
    
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789abcdef"[tmp_value - value * base];
    } while (value);
    
    *ptr-- = '\0';
    
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}
