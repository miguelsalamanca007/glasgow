#include "idt.h"
#include "klog.h"
#include "pic.h"
#include <stdint.h>

// Exception names for better error reporting
static const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt", 
    "Breakpoint",
    "Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present", 
    "Stack Fault",
    "General Protection Fault",
    "Page Fault"
};

// C exception handler called from assembly
void exception_handler(struct interrupt_frame* frame) {
    const char* exception_name = "Unknown Exception";
    
    if (frame->int_no < 15) {
        exception_name = exception_messages[frame->int_no];
    }
    
    // Log the exception details
    KERROR("CPU", "Exception %d (%s) occurred!", frame->int_no, exception_name);
    KERROR("CPU", "Error Code: 0x%x", frame->err_code);
    KERROR("CPU", "EIP: 0x%x, CS: 0x%x, EFLAGS: 0x%x", 
           frame->eip, frame->cs, frame->eflags);
    
    // Handle specific exceptions
    switch (frame->int_no) {
        case INT_DIVIDE_ERROR:
            KPANIC("IDT", "Division by zero detected!");
            break;
            
        case INT_INVALID_OPCODE:
            KPANIC("IDT", "Invalid opcode - unknown instruction!");
            break;
            
        case INT_DOUBLE_FAULT:
            KPANIC("IDT", "Double fault - critical system error!");
            break;
            
        case INT_GENERAL_PROTECTION:
            KPANIC("IDT", "General Protection Fault - memory access violation!");
            break;
            
        case INT_PAGE_FAULT: {
            uint32_t faulting_address;
            __asm__ volatile ("mov %%cr2, %0" : "=r" (faulting_address));
            KERROR("CPU", "Page fault at address 0x%x", faulting_address);
            KPANIC("IDT", "Page fault occurred!");
            break;
        }
        
        default:
            KPANIC("IDT", "Unhandled CPU exception!");
            break;
    }    
}

// Hardware interrupt handler (called from assembly)
void irq_handler(struct interrupt_frame* frame) {
    // Convert interrupt number back to IRQ number
    uint8_t irq = frame->int_no - 32;

    pic_send_eoi(irq);
   
    switch (irq) {
        case 0:  // Timer interrupt
            KDEBUG("IRQ", "Timer tick");
            break;
                
        case 1: {  // Keyboard interrupt
            // Leer el scancode del puerto del teclado
            uint8_t scancode = inb(0x60);
            KINFO("IRQ", "Key pressed! Scancode: 0x%x", scancode);
                
            // Convertir algunos scancodes básicos a caracteres para debug
            switch (scancode) {
                case 0x1E: KINFO("KBD", "Key: 'A'"); break;
                case 0x30: KINFO("KBD", "Key: 'B'"); break;
                case 0x2E: KINFO("KBD", "Key: 'C'"); break;
                case 0x20: KINFO("KBD", "Key: 'D'"); break;
                case 0x12: KINFO("KBD", "Key: 'E'"); break;
                case 0x02: KINFO("KBD", "Key: '1'"); break;
                case 0x03: KINFO("KBD", "Key: '2'"); break;
                case 0x1C: KINFO("KBD", "Key: 'ENTER'"); break;
                case 0x39: KINFO("KBD", "Key: 'SPACE'"); break;
                default:
                    if (scancode & 0x80) {
                        KDEBUG("KBD", "Key released: 0x%x", scancode & 0x7F);
                    } else {
                        KDEBUG("KBD", "Unknown key pressed: 0x%x", scancode);
                    }
                    break;
            }
            break;
        }
                    
            default:
                KWARN("IRQ", "Unhandled IRQ %d", irq);
                break;
        }
        
    // Send End of Interrupt signal
    pic_send_eoi(irq);
}
