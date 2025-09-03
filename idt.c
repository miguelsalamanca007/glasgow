#include "idt.h"
#include "klog.h"

static struct idt_entry idt_entries[NUMBER_OF_IDT_ENTRIES];

static struct idt_ptr idt_ptr;

void idt_init(void){
    KINFO("IDT", "Initializing Interrupt Descriptor Table...");

    idt_ptr.limit = (sizeof(struct idt_entry) * NUMBER_OF_IDT_ENTRIES - 1);
    idt_ptr.base = (uint32_t) &idt_entries; 

    for (int i = 0; i < NUMBER_OF_IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    idt_set_gate(0,  (uint32_t)isr0,  0x08, IDT_PRESENT | IDT_PRIVILEGE_0 | IDT_INTERRUPT);
    idt_set_gate(1,  (uint32_t)isr1,  0x08, IDT_PRESENT | IDT_PRIVILEGE_0 | IDT_INTERRUPT);
    idt_set_gate(3,  (uint32_t)isr3,  0x08, IDT_PRESENT | IDT_PRIVILEGE_0 | IDT_INTERRUPT);
    idt_set_gate(4,  (uint32_t)isr4,  0x08, IDT_PRESENT | IDT_PRIVILEGE_0 | IDT_INTERRUPT);
    idt_set_gate(6,  (uint32_t)isr6,  0x08, IDT_PRESENT | IDT_PRIVILEGE_0 | IDT_INTERRUPT);
    idt_set_gate(8,  (uint32_t)isr8,  0x08, IDT_PRESENT | IDT_PRIVILEGE_0 | IDT_INTERRUPT);
    idt_set_gate(13, (uint32_t)isr13, 0x08, IDT_PRESENT | IDT_PRIVILEGE_0 | IDT_INTERRUPT);
    idt_set_gate(14, (uint32_t)isr14, 0x08, IDT_PRESENT | IDT_PRIVILEGE_0 | IDT_INTERRUPT);

    KDEBUG("IDT", "Setting up hardware interrupt handlers...");
    idt_set_gate(32, (uint32_t)irq0, 0x08, IDT_PRESENT | IDT_PRIVILEGE_0 | IDT_INTERRUPT);
    idt_set_gate(33, (uint32_t)irq1, 0x08, IDT_PRESENT | IDT_PRIVILEGE_0 | IDT_INTERRUPT);

    idt_flush((uint32_t)&idt_ptr);
}


void idt_set_gate(uint32_t num, uint32_t base, uint16_t selector, uint8_t type) {

    if (num >= NUMBER_OF_IDT_ENTRIES) {
        KERROR("IDT", "Invalid IDT entry number: %d", num);
        return;
    }

    idt_entries[num].base_low = (base & 0xFFFF);
    idt_entries[num].selector = selector;
    idt_entries[num].reserved = 0x0;
    idt_entries[num].type = type;
    idt_entries[num].base_high = (base >> 16) & 0xFFFF;

}