OBJDIR = obj
ISODIR = iso
BOOTDIR = $(ISODIR)/boot
GRUBDIR = $(BOOTDIR)/grub
LINKER = linker.ld
KERNEL_ELF = $(BOOTDIR)/os.elf
ISO = k.iso
GRUB_CFG = $(GRUBDIR)/grub.cfg
CPPFLAGS = -I./include

SRC_C := $(wildcard src/*.c)
SRC_S := $(wildcard src/*.s)
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
	i686-elf-gcc -g $(CPPFLAGS) -c $< -o $@ -ffreestanding -Wall

$(OBJDIR)/%.o: src/%.s | $(OBJDIR)
	nasm -felf32 $< -o $@

$(KERNEL_ELF): $(KERNEL_OBJ) $(LINKER) | $(BOOTDIR)
	i686-elf-gcc -g -T $(LINKER) -o $@ -ffreestanding -O2 -nostdlib $(KERNEL_OBJ) -lgcc

$(ISO): $(KERNEL_ELF) $(GRUB_CFG)
	sudo grub-mkrescue -o $@ $(ISODIR)

all: $(ISO)

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -boot d -m 2048 -smp 4 -d int -serial stdio

clean:
	rm -rf $(OBJDIR) $(BOOTDIR)/os.elf

fclean: clean
	rm -rf $(ISO)

re: fclean all

.PHONY: all run clean fclean re