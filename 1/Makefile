# Define directories and files locations
OBJDIR = obj
ISODIR = iso
BOOTDIR = $(ISODIR)/boot
GRUBDIR = $(BOOTDIR)/grub
LINKER = linker.ld
KERNEL_ELF = $(BOOTDIR)/kernel.elf
ISO = k.iso
GRUB_CFG = $(GRUBDIR)/grub.cfg

# Tools and flags
AS = nasm
CC = gcc
LD = ld
ASFLAGS = -f elf32
CPPFLAGS = -I./include -m32 -ffreestanding -fno-builtin -fno-stack-protector -nostdlib -c
LDFLAGS = -m elf_i386 -T $(LINKER)

# find source files and process them
SRC_C := $(shell find src -type f -name '*.c')
SRC_S := $(shell find src -type f -name '*.s')
OBJ_C := $(patsubst src/%.c,$(OBJDIR)/%.o,$(SRC_C))
OBJ_S := $(patsubst src/%.s,$(OBJDIR)/%.o,$(SRC_S))
KERNEL_OBJ := $(OBJ_C) $(OBJ_S)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BOOTDIR):
	mkdir -p $(BOOTDIR)

$(GRUBDIR):
	mkdir -p $(GRUBDIR)

$(OBJDIR)/%.o: src/%.c | $(OBJDIR)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(OBJDIR)/%.o: src/%.s | $(OBJDIR)
	$(AS) $(ASFLAGS) $< -o $@

$(KERNEL_ELF): $(KERNEL_OBJ) $(LINKER) | $(BOOTDIR)
	$(LD) $(LDFLAGS) -o $@ $(KERNEL_OBJ)

$(ISO): $(KERNEL_ELF) $(GRUB_CFG)
	grub-mkrescue -o $(ISO) iso/ --locales="" --fonts="" --themes="" \
		--modules="multiboot normal iso9660 biosdisk ls" \
		--directory=$(HOME)/local/usr/lib/grub/i386-pc

all: $(ISO)

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -boot d -m 2048 -smp 4 -serial stdio

clean:
	rm -rf $(OBJDIR) $(BOOTDIR)/os.elf

fclean: clean
	rm -rf $(ISO)

re: fclean all

.PHONY: all run clean fclean re