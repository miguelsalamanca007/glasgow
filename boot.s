# boot.s - Multiboot compatible bootloader
# This sets up the initial environment for our kernel

.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a multiboot header that marks the program as a kernel
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Reserve a stack for the initial thread
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# The kernel entry point
.section .text
.global _start
.type _start, @function
_start:
	# The bootloader has loaded us into 32-bit protected mode
	# Interrupts are disabled, paging is disabled
	# The processor state is as defined in the multiboot standard
	
	# To set up a stack, we set the esp register to point to the top of the
	# stack (as it grows downwards on x86 systems)
	mov $stack_top, %esp

	# This is a good place to initialize crucial processor state before the
	# high-level kernel is entered. It's best to minimize the early
	# environment where crucial features are offline. Note that the
	# processor is not fully initialized yet: Features such as floating
	# point instructions and instruction set extensions are not initialized
	# yet. The GDT should be loaded here. Paging should be enabled here.
	# C++ features such as global constructors and exceptions will require
	# runtime support to work as well.

	# Enter the high-level kernel
	call kernel_main

	# If the system has nothing more to do, put the computer into an
	# infinite loop. To do that:
	# 1) Disable interrupts with cli (clear interrupt enable in eflags)
	# 2) Wait for the next interrupt to arrive with hlt (halt instruction)
	# 3) Jump to the hlt instruction if it ever wakes up due to a
	#    non-maskable interrupt occurring or due to system management mode
	cli
1:	hlt
	jmp 1b

# Set the size of the _start symbol to the current location '.' minus its start
.size _start, . - _start
