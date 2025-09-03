#ifndef GDT_H
#define GDT_H

#include <stdint.h>

// GDT Entry structure (8 bytes each)
struct gdt_entry {
    uint16_t limit_low;     // Lower 16 bits of the limit
    uint16_t base_low;      // Lower 16 bits of the base address  
    uint8_t  base_middle;   // Next 8 bits of the base address
    uint8_t  access;        // Access flags (present, privilege, type)
    uint8_t  granularity;   // Granularity flags and upper limit bits
    uint8_t  base_high;     // Upper 8 bits of the base address
} __attribute__((packed));  // Prevent compiler padding

// GDT Pointer structure for LGDT instruction (to be passed to the CPU)
struct gdt_ptr {
    uint16_t limit;         // Size of GDT - 1
    uint32_t base;          // Base address of GDT
} __attribute__((packed));

// Access byte flags
#define GDT_PRESENT     0x80    // Segment is present in memory
#define GDT_PRIVILEGE_0 0x00    // Ring 0 (kernel mode)
#define GDT_PRIVILEGE_1 0x20    // Ring 1 (unused)  
#define GDT_PRIVILEGE_2 0x40    // Ring 2 (unused)
#define GDT_PRIVILEGE_3 0x60    // Ring 3 (user mode)
#define GDT_EXECUTABLE  0x08    // Executable segment (code)
#define GDT_WRITABLE    0x02    // Writable segment (data)
#define GDT_READABLE    0x02    // Readable segment (code)

// Granularity byte flags  
#define GDT_GRANULARITY 0x80    // Limit is in 4KB blocks
#define GDT_32BIT       0x40    // 32-bit protected mode segment
#define GDT_16BIT       0x00    // 16-bit segment

// Segment selectors (used to load segments)
#define KERNEL_CODE_SEGMENT 0x08    // GDT entry 1
#define KERNEL_DATA_SEGMENT 0x10    // GDT entry 2  
#define USER_CODE_SEGMENT   0x18    // GDT entry 3
#define USER_DATA_SEGMENT   0x20    // GDT entry 4

// Number of GDT entries
#define GDT_ENTRIES 5

// Initialize the Global Descriptor Table
void gdt_init(void);

// Set a GDT entry with specified parameters
void gdt_set_gate(uint32_t num, uint32_t base, uint32_t limit, 
                  uint8_t access, uint8_t gran);

// Assembly function to load the GDT (defined in gdt_asm.s)
extern void gdt_flush(uint32_t gdt_ptr_addr);

#endif // GDT_H
