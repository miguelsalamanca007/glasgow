# gdt_asm.s - Assembly helper for loading GDT
# This must be in assembly because we need to reload segment registers

.section .text
.global gdt_flush
.type gdt_flush, @function

gdt_flush:
    # Parameter: GDT pointer address is in 4(%esp)
    mov 4(%esp), %eax       # Get GDT pointer address from stack
    lgdt (%eax)             # Load GDT using LGDT instruction
    
    # Reload code segment by doing a far jump
    # We jump to the next instruction but with new code segment
    ljmp $0x08, $reload_segments
    
reload_segments:
    # Now we're using the new code segment (0x08)
    # Reload all data segment registers with new data segment (0x10)
    mov $0x10, %ax          # Load kernel data segment selector
    mov %ax, %ds            # Data segment
    mov %ax, %es            # Extra segment  
    mov %ax, %fs            # F segment
    mov %ax, %gs            # G segment
    mov %ax, %ss            # Stack segment
    
    # Return to caller - now using new GDT!
    ret

.size gdt_flush, . - gdt_flush