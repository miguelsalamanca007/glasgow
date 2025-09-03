#ifndef PIC_H
#define PIC_H

#include <stdint.h>

// PIC port addresses
#define PIC1_COMMAND    0x20    // Master PIC command port
#define PIC1_DATA       0x21    // Master PIC data port
#define PIC2_COMMAND    0xA0    // Slave PIC command port  
#define PIC2_DATA       0xA1    // Slave PIC data port

// PIC commands
#define PIC_EOI         0x20    // End of Interrupt command

// ICW1 (Initialization Control Word 1)
#define ICW1_ICW4       0x01    // ICW4 needed
#define ICW1_SINGLE     0x02    // Single (cascade) mode
#define ICW1_INTERVAL4  0x04    // Call address interval 4 (8)
#define ICW1_LEVEL      0x08    // Level triggered (edge) mode
#define ICW1_INIT       0x10    // Initialization required

// ICW4 (Initialization Control Word 4)
#define ICW4_8086       0x01    // 8086/88 (MCS-80/85) mode
#define ICW4_AUTO       0x02    // Auto (normal) EOI
#define ICW4_BUF_SLAVE  0x08    // Buffered mode/slave
#define ICW4_BUF_MASTER 0x0C    // Buffered mode/master
#define ICW4_SFNM       0x10    // Special fully nested

// IRQ numbers (after remapping to 32-47)
#define IRQ_TIMER       32      // IRQ 0 - System timer
#define IRQ_KEYBOARD    33      // IRQ 1 - Keyboard
#define IRQ_CASCADE     34      // IRQ 2 - Cascade for slave PIC
#define IRQ_COM2        35      // IRQ 3 - COM2
#define IRQ_COM1        36      // IRQ 4 - COM1
#define IRQ_LPT2        37      // IRQ 5 - LPT2
#define IRQ_FLOPPY      38      // IRQ 6 - Floppy disk
#define IRQ_LPT1        39      // IRQ 7 - LPT1
#define IRQ_RTC         40      // IRQ 8 - Real-time clock
#define IRQ_FREE1       41      // IRQ 9 - Free
#define IRQ_FREE2       42      // IRQ 10 - Free
#define IRQ_FREE3       43      // IRQ 11 - Free
#define IRQ_MOUSE       44      // IRQ 12 - PS/2 Mouse
#define IRQ_FPU         45      // IRQ 13 - FPU
#define IRQ_PRIMARY_ATA 46      // IRQ 14 - Primary ATA
#define IRQ_SECONDARY_ATA 47    // IRQ 15 - Secondary ATA

// Function prototypes
void pic_init(void);
void pic_send_eoi(uint8_t irq);
void pic_disable(void);
void irq_set_mask(uint8_t irq_line);
void irq_clear_mask(uint8_t irq_line);

// Port I/O functions
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

#endif // PIC_H
