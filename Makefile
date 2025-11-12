# === Compilation tools ===
AS      = nasm
CC      = gcc
LD      = ld
ASFLAGS = -f elf32
CFLAGS  = -W -W -W -m32 -ffreestanding -fno-builtin -fno-stack-protector -nostdlib -I src/kernel/include/ -c
LDFLAGS = -m elf_i386 -T linker.ld

# === Files and directories ===
SRC_DIR     = src
BOOT_DIR    = $(SRC_DIR)/boot
KERNEL_DIR  = $(SRC_DIR)/kernel/src
INCLUDE_DIR = $(SRC_DIR)/kernel/include
BUILD_DIR   = build
ISO_DIR     = iso
GRUB_DIR    = grub
ISO         = kernel.iso
ELF         = kernel.elf

# === Automatically search for source files ===
C_SRCS  = $(shell find $(KERNEL_DIR) -name '*.c')
ASM_SRCS = $(shell find $(BOOT_DIR) -name '*.s')
OBJS    = $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(C_SRCS:.c=.o) $(ASM_SRCS:.s=.o))

# === Default target ===
all: $(ISO)

# === ASM files compilation ===
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# === C files compilation ===
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

# === Links editing ===
$(ELF): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(ELF)

# === ISO Image build ===
$(ISO): $(ELF)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(ELF) $(ISO_DIR)/boot/
	cp $(GRUB_DIR)/grub.cfg $(ISO_DIR)/boot/grub/
	grub-mkrescue -o $(ISO) $(ISO_DIR)/ --locales="" --fonts="" --themes="" \
		--modules="multiboot normal iso9660 biosdisk ls" \
		--directory=$(HOME)/local/usr/lib/grub/i386-pc

# === Launch with QEMU ===
run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -serial stdio

# === Basic cleaning ===
clean:
	rm -rf $(BUILD_DIR) $(ELF)

# === Full cleaning ===
fclean: clean
	rm -f $(ISO)
	rm -rf $(ISO_DIR)

# === Full rebuild ===
re: fclean all

.PHONY: all clean fclean re run


#mkdir -p ~/local
#apt download grub-pc-bin
#dpkg-deb -x grub-pc-bin*.deb ~/local/
#rm -f grub-pc-bin*.deb