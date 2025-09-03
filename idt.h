#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t reserved;
    uint8_t type;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Interrupt frame structure (pushed by CPU and our assembly code)
struct interrupt_frame {
    uint32_t ds;        // Data segment
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  // General purpose registers (pusha)
    uint32_t int_no;    // Interrupt number
    uint32_t err_code;  // Error code
    uint32_t eip, cs, eflags, useresp, ss;  // Pushed by CPU
};

// Set a new idt gate
void idt_set_gate(uint32_t num, uint32_t base, uint16_t selector, uint8_t type);

// Gate types
#define IDT_PRIVILEGE_0 0x00
#define TASK_GATE 0x5
#define INTERRUPT_GATE_16 0x6
#define TRAP_GATE_16 0x7
#define IDT_PRESENT 0x80
#define IDT_INTERRUPT 0x0E
#define TRAP_GATE_32 0xF

// Standard interrupt/exception numbers
#define INT_DIVIDE_ERROR        0   // Division by zero
#define INT_DEBUG               1   // Debug exception
#define INT_BREAKPOINT          3   // Breakpoint
#define INT_OVERFLOW            4   // Overflow
#define INT_INVALID_OPCODE      6   // Invalid opcode
#define INT_DOUBLE_FAULT        8   // Double fault
#define INT_GENERAL_PROTECTION  13  // General protection fault
#define INT_PAGE_FAULT          14  // Page fault

// Number of entries
#define NUMBER_OF_IDT_ENTRIES 256

// Initialize idt
void idt_init(void);

extern void idt_flush(uint32_t idt_ptr);

// Exception handlers
extern void isr0(void);   // Division by zero
extern void isr1(void);   // Debug
extern void isr3(void);   // Breakpoint
extern void isr4(void);   // Overflow
extern void isr6(void);   // Invalid opcode
extern void isr8(void);   // Double fault
extern void isr13(void);  // General protection fault
extern void isr14(void);  // Page fault

// Hardware interrupt handlers
extern void irq0(void);   // Timer
extern void irq1(void);   // Keyboard


#endif