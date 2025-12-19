.SUFFIXES:

override OUTPUT := myos
ISO_IMAGE := image.iso
ISO_ROOT := iso_root
LIMINE_DIR := limine

TOOLCHAIN :=
TOOLCHAIN_PREFIX :=

ifneq ($(TOOLCHAIN),)
    ifeq ($(TOOLCHAIN_PREFIX),)
        TOOLCHAIN_PREFIX := $(TOOLCHAIN)-
    endif
endif

ifneq ($(TOOLCHAIN_PREFIX),)
    CC := $(TOOLCHAIN_PREFIX)gcc
    LD := $(TOOLCHAIN_PREFIX)ld
else
    CC := cc
    LD := ld
endif

ifeq ($(TOOLCHAIN),llvm)
    CC := clang
    LD := ld.lld
endif

CFLAGS := -g -O2 -pipe
CPPFLAGS :=
NASMFLAGS := -g
LDFLAGS :=

override CC_IS_CLANG := $(shell ! $(CC) --version 2>/dev/null | grep -q '^Target: '; echo $$?)

ifeq ($(CC_IS_CLANG),1)
    override CC += -target x86_64-unknown-none-elf
endif

override CFLAGS += \
    -Wall \
    -Wextra \
    -std=gnu11 \
    -ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-lto \
    -fno-PIC \
    -ffunction-sections \
    -fdata-sections \
    -m64 \
    -march=x86-64 \
    -mabi=sysv \
    -mno-80387 \
    -mno-mmx \
    -mno-sse \
    -mno-sse2 \
    -mno-red-zone \
    -mcmodel=kernel

override CPPFLAGS := \
    -I kernel \
    $(CPPFLAGS) \
    -MMD \
    -MP

override NASMFLAGS := \
    -f elf64 \
    $(patsubst -g,-g -F dwarf,$(NASMFLAGS)) \
    -Wall

override LDFLAGS += \
    -m elf_x86_64 \
    -nostdlib \
    -static \
    -z max-page-size=0x1000 \
    --gc-sections \
    -T linker.lds

override SRCFILES := $(shell find -L kernel -type f 2>/dev/null | LC_ALL=C sort)
override CFILES := $(filter %.c,$(SRCFILES))
override ASFILES := $(filter %.S,$(SRCFILES))
override NASMFILES := $(filter %.asm,$(SRCFILES))

override OBJ := $(addprefix obj/, \
    $(CFILES:.c=.c.o) \
    $(ASFILES:.S=.S.o) \
    $(NASMFILES:.asm=.asm.o))

override HEADER_DEPS := $(addprefix obj/, \
    $(CFILES:.c=.c.d) \
    $(ASFILES:.S=.S.d))

.PHONY: all
all: bin/$(OUTPUT)

-include $(HEADER_DEPS)

bin/$(OUTPUT): GNUmakefile linker.lds $(OBJ)
	mkdir -p "$(dir $@)"
	$(LD) $(LDFLAGS) $(OBJ) -o $@

obj/%.c.o: %.c GNUmakefile
	mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

obj/%.S.o: %.S GNUmakefile
	mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

obj/%.asm.o: %.asm GNUmakefile
	mkdir -p "$(dir $@)"
	nasm $(NASMFLAGS) $< -o $@

.PHONY: limine
limine:
	if [ ! -d "$(LIMINE_DIR)" ]; then \
		git clone https://codeberg.org/Limine/Limine.git $(LIMINE_DIR) \
			--branch=v10.x-binary --depth=1; \
	fi
	$(MAKE) -C $(LIMINE_DIR)

.PHONY: iso_root
iso_root: bin/$(OUTPUT) limine
	rm -rf $(ISO_ROOT)
	mkdir -p $(ISO_ROOT)/boot/limine
	mkdir -p $(ISO_ROOT)/EFI/BOOT

	cp bin/$(OUTPUT) $(ISO_ROOT)/boot/

	cp limine.conf \
	   $(LIMINE_DIR)/bin/limine-bios.sys \
	   $(LIMINE_DIR)/bin/limine-bios-cd.bin \
	   $(LIMINE_DIR)/bin/limine-uefi-cd.bin \
	   $(ISO_ROOT)/boot/limine/


	cp $(LIMINE_DIR)/bin/BOOTX64.EFI $(ISO_ROOT)/EFI/BOOT/
	cp $(LIMINE_DIR)/bin/BOOTIA32.EFI $(ISO_ROOT)/EFI/BOOT/

.PHONY: iso
iso: iso_root
	xorriso -as mkisofs -R -r -J \
		-b boot/limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		-hfsplus \
		-apm-block-size 2048 \
		--efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part \
		--efi-boot-image \
		--protective-msdos-label \
		$(ISO_ROOT) -o $(ISO_IMAGE)

	#./$(LIMINE_DIR) sudo limine bios-install ../$(ISO_IMAGE)

.PHONY: run
run: iso
	qemu-system-x86_64 \
		-cdrom $(ISO_IMAGE) \
		-m 512M \
		-serial stdio \
		-boot d

.PHONY: clean
clean:
	rm -rf bin obj $(ISO_ROOT) $(ISO_IMAGE)
