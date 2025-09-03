# interrupts.s - Assembly interrupt handlers and stubs

.section .text

# Macro to create interrupt service routine stubs
.macro ISR_NOERRCODE num
    .global isr\num
    isr\num:
        push $0          # Push dummy error code
        push $\num       # Push interrupt number
        jmp isr_common   # Jump to common handler
.endm

.macro ISR_ERRCODE num
    .global isr\num
    isr\num:
        push $\num       # Push interrupt number (error code already pushed)
        jmp isr_common   # Jump to common handler
.endm

# Macro for hardware interrupt (IRQ) stubs  
.macro IRQ num, irq_num
    .global irq\num
    irq\num:
        push $0          # Push dummy error code
        push $\irq_num   # Push interrupt number (32 + IRQ number)
        jmp irq_common   # Jump to common IRQ handler
.endm

# Create all the ISR stubs
ISR_NOERRCODE 0   # Division by zero
ISR_NOERRCODE 1   # Debug
ISR_NOERRCODE 3   # Breakpoint
ISR_NOERRCODE 4   # Overflow
ISR_NOERRCODE 6   # Invalid opcode
ISR_ERRCODE   8   # Double fault
ISR_ERRCODE   13  # General protection fault
ISR_ERRCODE   14  # Page fault

# Create IRQ stubs
IRQ 0, 32  # Timer (IRQ 0 -> INT 32)
IRQ 1, 33  # Keyboard (IRQ 1 -> INT 33)

# Common ISR handler
isr_common:
    # Save all general-purpose registers
    pusha
    
    # Save data segments
    mov %ds, %ax
    push %eax
    
    # Load kernel data segment
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    # Push pointer to interrupt frame as parameter
    push %esp
    
    # Call C exception handler
    call exception_handler

    # Remove the frame pointer parameter

    add $4, %esp
    
    # Restore data segments
    pop %eax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    
    # Restore all registers
    popa
    
    # Clean up error code and interrupt number
    add $8, %esp
    
    # Return from interrupt
    iret

# Common IRQ handler
irq_common:
    # Save all general-purpose registers
    pusha
    
    # Save data segments
    mov %ds, %ax
    push %eax
    
    # Load kernel data segment
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    # push %esp
    
    # Call C interrupt handler
    call irq_handler

    # Remove the frame pointer parameter
    add $4, %esp
    
    # Restore data segments
    pop %eax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    
    # Restore all registers
    popa
    
    # Clean up error code and interrupt number
    add $8, %esp
    
    # Return from interrupt
    iret

# Assembly function to load IDT
.global idt_flush
idt_flush:
    mov 4(%esp), %eax    # Get IDT pointer from stack
    lidt (%eax)          # Load IDT
    ret
