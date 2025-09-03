#include "gdt.h"
#include "klog.h"

static struct gdt_entry gdt_entries[GDT_ENTRIES];

static struct gdt_ptr gdt_ptr;

void gdt_init(void) {
    KINFO("GDT", "Initializing Global Descriptor Table...");
    
    gdt_ptr.limit = (sizeof(struct gdt_entry) * GDT_ENTRIES) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;
    
    KDEBUG("GDT", "GDT base address: 0x%x", gdt_ptr.base);
    KDEBUG("GDT", "GDT limit: %d bytes", gdt_ptr.limit + 1);
    
    // Set up GDT entries:
    
    // Entry 0: Null descriptor (required by x86)
    gdt_set_gate(0, 0, 0, 0, 0);
    KDEBUG("GDT", "Entry 0: Null descriptor");
    
    // Entry 1: Kernel code segment (Ring 0)
    // Base: 0x00000000, Limit: 0xFFFFFFFF (4GB)
    // Access: Present | Ring 0 | Code | Executable | Readable
    // Granularity: 4KB blocks | 32-bit
    gdt_set_gate(1, 0, 0xFFFFFFFF,
                 GDT_PRESENT | GDT_PRIVILEGE_0 | 0x10 | GDT_EXECUTABLE | GDT_READABLE,
                 GDT_GRANULARITY | GDT_32BIT | 0x0F);
    KDEBUG("GDT", "Entry 1: Kernel code segment (0x08)");
    
    // Entry 2: Kernel data segment (Ring 0)  
    // Base: 0x00000000, Limit: 0xFFFFFFFF (4GB)
    // Access: Present | Ring 0 | Data | Writable
    // Granularity: 4KB blocks | 32-bit
    gdt_set_gate(2, 0, 0xFFFFFFFF,
                 GDT_PRESENT | GDT_PRIVILEGE_0 | 0x10 | GDT_WRITABLE,
                 GDT_GRANULARITY | GDT_32BIT | 0x0F);
    KDEBUG("GDT", "Entry 2: Kernel data segment (0x10)");
    
    // Entry 3: User code segment (Ring 3) - for future user programs
    gdt_set_gate(3, 0, 0xFFFFFFFF,
                 GDT_PRESENT | GDT_PRIVILEGE_3 | 0x10 | GDT_EXECUTABLE | GDT_READABLE,
                 GDT_GRANULARITY | GDT_32BIT | 0x0F);  
    KDEBUG("GDT", "Entry 3: User code segment (0x18)");
    
    // Entry 4: User data segment (Ring 3) - for future user programs
    gdt_set_gate(4, 0, 0xFFFFFFFF,
                 GDT_PRESENT | GDT_PRIVILEGE_3 | 0x10 | GDT_WRITABLE,
                 GDT_GRANULARITY | GDT_32BIT | 0x0F);
    KDEBUG("GDT", "Entry 4: User data segment (0x20)");
    
    // Load the GDT using assembly helper
    KINFO("GDT", "Loading new GDT...");
    gdt_flush((uint32_t)&gdt_ptr);
    
    KINFO("GDT", "GDT loaded successfully! Kernel now using custom segments.");
}

void gdt_set_gate(uint32_t num, uint32_t base, uint32_t limit,
                  uint8_t access, uint8_t gran) {
    
    if (num >= GDT_ENTRIES) {
        KERROR("GDT", "Invalid GDT entry number: %d", num);
        return;
    }
    
    // Set base address
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;
    
    // Set limit  
    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;
    
    // Set granularity and access flags
    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access       = access;
    
    KDEBUG("GDT", "Set entry %d: base=0x%x, limit=0x%x, access=0x%x, gran=0x%x", 
           num, base, limit, access, gran);
}