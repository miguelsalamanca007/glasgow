# Makefile for SimpleOS Kernel

# Default target architecture
ARCH ?= i686

# Toolchain
AS = $(ARCH)-elf-as
CC = $(ARCH)-elf-gcc
LD = $(ARCH)-elf-gcc

# Directories
SRCDIR = .
BUILDDIR = build
ISODIR = isodir

# Flags
ASFLAGS = 
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Wno-unused-parameter
LDFLAGS = -ffreestanding -O2 -nostdlib

# Source files
ASM_SOURCES = boot.s
C_SOURCES = kernel.c
SOURCES = $(ASM_SOURCES) $(C_SOURCES)

# Object files
ASM_OBJECTS = $(ASM_SOURCES:%.s=$(BUILDDIR)/%.o)
C_OBJECTS = $(C_SOURCES:%.c=$(BUILDDIR)/%.o)
OBJECTS = $(ASM_OBJECTS) $(C_OBJECTS)

# Output
KERNEL = $(BUILDDIR)/mykernel.bin
ISO = $(BUILDDIR)/mykernel.iso

.PHONY: all clean run debug iso install-deps check-deps

# Default target
all: check-deps $(KERNEL)

# Create build directory
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Compile assembly files
$(BUILDDIR)/%.o: %.s | $(BUILDDIR)
	$(AS) $(ASFLAGS) -o $@ $<

# Compile C files
$(BUILDDIR)/%.o: %.c | $(BUILDDIR)
	$(CC) -c $< -o $@ $(CFLAGS)

# Link kernel
$(KERNEL): $(OBJECTS) linker.ld
	$(LD) -T linker.ld -o $@ $(LDFLAGS) $(OBJECTS) -lgcc

# Create ISO for GRUB
iso: $(ISO)

$(ISO): $(KERNEL)
	mkdir -p $(ISODIR)/boot/grub
	cp $(KERNEL) $(ISODIR)/boot/mykernel.bin
	echo 'menuentry "SimpleOS" {' > $(ISODIR)/boot/grub/grub.cfg
	echo '	multiboot /boot/mykernel.bin' >> $(ISODIR)/boot/grub/grub.cfg
	echo '}' >> $(ISODIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) $(ISODIR)

# Run in QEMU
run: $(KERNEL)
	qemu-system-i386 -kernel $(KERNEL) -m 512M -serial stdio

# Debug with QEMU + GDB
debug: $(KERNEL)
	@echo "Starting QEMU with GDB server on port 1234"
	@echo "In another terminal, run: gdb -ex 'target remote localhost:1234' -ex 'symbol-file $(KERNEL)'"
	qemu-system-i386 -kernel $(KERNEL) -m 512M -s -S

# Clean build files
clean:
	rm -rf $(BUILDDIR) $(ISODIR)

# Check if required tools are installed
check-deps:
	@which $(CC) > /dev/null || (echo "Error: $(CC) not found. Please install cross-compiler." && exit 1)
	@which qemu-system-i386 > /dev/null || (echo "Error: qemu-system-i386 not found. Please install QEMU." && exit 1)
	@echo "All dependencies found!"

# Install dependencies on macOS
install-deps:
	@echo "Installing dependencies for macOS..."
	@echo "This will install: nasm, qemu, and cross-compiler"
	brew install nasm qemu
	@echo "Building cross-compiler (this may take a while)..."
	@echo "You may need to build i686-elf-gcc manually if not available in brew"
	@echo "See: https://wiki.osdev.org/GCC_Cross-Compiler"

# Show kernel information
info: $(KERNEL)
	@echo "Kernel: $(KERNEL)"
	@echo "Size: $$(du -h $(KERNEL) | cut -f1)"
	@file $(KERNEL)
	@echo ""
	@echo "Sections:"
	@objdump -h $(KERNEL)

# Disassemble kernel
disasm: $(KERNEL)
	objdump -D $(KERNEL) > $(BUILDDIR)/kernel.asm
	@echo "Disassembly saved to $(BUILDDIR)/kernel.asm"
