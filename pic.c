#include "pic.h"
#include "klog.h"

void pic_init(void) {
    KINFO("PIC", "Initializing Programmable Interrupt Controller...");
    
    // Save current interrupt masks
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);
    
    KDEBUG("PIC", "Current masks - Master: 0x%x, Slave: 0x%x", mask1, mask2);
    
    // Start initialization sequence (ICW1)
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // Master PIC
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);  // Slave PIC
    
    // ICW2: Set interrupt vector offsets
    // Master PIC: IRQ 0-7 mapped to interrupts 32-39
    // Slave PIC: IRQ 8-15 mapped to interrupts 40-47
    outb(PIC1_DATA, 32);  // Master PIC offset
    outb(PIC2_DATA, 40);  // Slave PIC offset
    
    // ICW3: Set up cascade
    outb(PIC1_DATA, 4);   // Tell master that slave is at IRQ2 (0000 0100)
    outb(PIC2_DATA, 2);   // Tell slave its cascade identity (0000 0010)
    
    // ICW4: Set mode
    outb(PIC1_DATA, ICW4_8086);  // 8086 mode
    outb(PIC2_DATA, ICW4_8086);  // 8086 mode
    
    // Restore interrupt masks (initially disable all)
    outb(PIC1_DATA, 0xFF);  // Mask all master PIC interrupts
    outb(PIC2_DATA, 0xFF);  // Mask all slave PIC interrupts
    
    KINFO("PIC", "PIC initialized - IRQs remapped to 32-47");
    KDEBUG("PIC", "Master PIC: IRQ 0-7 -> INT 32-39");
    KDEBUG("PIC", "Slave PIC: IRQ 8-15 -> INT 40-47");
    
    // Enable only timer and keyboard for now
    irq_clear_mask(0);  // Enable timer (IRQ 0)
    irq_clear_mask(1);  // Enable keyboard (IRQ 1)
    irq_clear_mask(2);  // Enable cascade (required for slave PIC)
    
    KINFO("PIC", "Enabled IRQ 0 (timer) and IRQ 1 (keyboard)");
}

void pic_send_eoi(uint8_t irq) {
    // If IRQ came from slave PIC (IRQ 8-15), send EOI to both
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    // Always send EOI to master PIC
    outb(PIC1_COMMAND, PIC_EOI);
}

void pic_disable(void) {
    KINFO("PIC", "Disabling PIC...");
    outb(PIC1_DATA, 0xFF);  // Mask all interrupts
    outb(PIC2_DATA, 0xFF);
}

void irq_set_mask(uint8_t irq_line) {
    uint16_t port;
    
    if (irq_line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_line -= 8;
    }
    
    uint8_t value = inb(port) | (1 << irq_line);
    outb(port, value);
    
    KDEBUG("PIC", "Masked IRQ %d", irq_line < 8 ? irq_line : irq_line + 8);
}

void irq_clear_mask(uint8_t irq_line) {
    uint16_t port;
    uint8_t original_irq = irq_line;
    
    if (irq_line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_line -= 8;
    }
    
    uint8_t value = inb(port) & ~(1 << irq_line);
    outb(port, value);
    
    KDEBUG("PIC", "Unmasked IRQ %d", original_irq);
}
