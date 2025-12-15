CC = gcc
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra
LD = ld
LDFLAGS = -m elf_i386

SRC = kernel/kernel.c kernel/console.c kernel/string.c kernel/keyboard.c kernel/readline.c kernel/memory.c
OBJ = $(SRC:.c=.o)

all: kernel.bin

kernel.bin: $(OBJ) linker.ld
	$(LD) $(LDFLAGS) -T linker.ld -o $@ $(OBJ)

run: iso
	qemu-system-i386 -cdrom myos.iso -boot d

iso: kernel.bin
	mkdir -p iso/boot/grub
	cp kernel.bin iso/boot/
	cp boot/grub.cfg iso/boot/grub/
	grub-mkrescue -o myos.iso iso/

clean:
	rm -rf *.o kernel/*.o iso kernel.bin myos.iso
