CC = gcc
AS = nasm
LD = ld

CFLAGS = -m64 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -n -T linker.ld

# Find all C and ASM files
C_SRC  := $(wildcard kernel/*.c)
ASM_SRC:= $(wildcard kernel/*.asm)

# Convert source files to object files
C_OBJ  := $(C_SRC:.c=.o)
ASM_OBJ:= $(ASM_SRC:.asm=.o)

OBJ := $(C_OBJ) $(ASM_OBJ)

all: iso

# Assemble ASM files
kernel/%.o: kernel/%.asm
	$(AS) -f elf64 $< -o $@

# Compile C files
kernel/%.o: kernel/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link kernel
kernel.elf: $(OBJ)
	$(LD) $(LDFLAGS) $^ -o $@

# Build ISO
iso: kernel.elf
	mkdir -p iso/boot/grub
	cp kernel.elf iso/boot/kernel.elf
	cp boot/grub/grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o os.iso iso

# Run in QEMU
run: iso
	qemu-system-x86_64 -cdrom os.iso -boot d -no-reboot -d int,guest_errors

# Clean
clean:
	rm -rf iso kernel/*.o kernel.elf os.iso
