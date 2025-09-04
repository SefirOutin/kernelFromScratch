OBJDIR = obj
ISODIR = iso
BOOTDIR = $(ISODIR)/boot
GRUBDIR = $(BOOTDIR)/grub
KERNEL_SRC = src/kernel_main.c
MULTIBOOT_SRC = src/multiboot.s
KERNEL_OBJ = $(OBJDIR)/kernel.o
MULTIBOOT_OBJ = $(OBJDIR)/multiboot.o
LINKER = linker.ld
KERNEL_ELF = $(BOOTDIR)/os.elf
ISO = k.iso
GRUB_CFG = $(GRUBDIR)/grub.cfg

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BOOTDIR):
	mkdir -p $(BOOTDIR)

$(GRUBDIR):
	mkdir -p $(GRUBDIR)

$(KERNEL_OBJ): $(KERNEL_SRC) | $(OBJDIR)
	i686-elf-gcc -g -c $< -o $@ -ffreestanding -W -W -W

$(MULTIBOOT_OBJ): $(MULTIBOOT_SRC) | $(OBJDIR)
	nasm -felf32 $< -o $@

$(KERNEL_ELF): $(KERNEL_OBJ) $(MULTIBOOT_OBJ) $(LINKER) | $(BOOTDIR)
	i686-elf-gcc -g -T $(LINKER) -o $@ -ffreestanding -O2 -nostdlib $(KERNEL_OBJ) $(MULTIBOOT_OBJ) -lgcc

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