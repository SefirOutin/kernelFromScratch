# === Files and directories ===
SRC_DIR     := src
BOOT_DIR    := $(SRC_DIR)/boot
KERNEL_DIR  := $(SRC_DIR)/kernel/modules
BUILD_DIR   := build
ISO_DIR     := iso
GRUB_DIR    := grub
ISO         := kernel.iso
ELF         := kernel.elf

# === Compilation tools ===
AS      := nasm
CC      := gcc
LD      := ld
ASFLAGS := -f elf32
CFLAGS  := -W -Wall -m32 -g -ffreestanding -fno-builtin -fno-stack-protector -nostdlib \
           -c \
           -MMD -MP \
           -I $(SRC_DIR)/kernel/include \
           -I $(SRC_DIR)/kernel/modules/core/include \
           -I $(SRC_DIR)/kernel/modules/arch/include \
           -I $(SRC_DIR)/kernel/modules/drivers/include \
           -I $(SRC_DIR)/kernel/modules/lib/include \
           -I $(SRC_DIR)/kernel/modules/memory/include \
           -I $(SRC_DIR)/kernel/modules/shell/include
LDFLAGS := -m elf_i386 -T linker.ld

# === Automatically find sources ===
C_SRCS   := $(shell find $(SRC_DIR) -name '*.c')
ASM_SRCS := $(shell find $(SRC_DIR) -name '*.s')
OBJS     := $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(C_SRCS:.c=.o) $(ASM_SRCS:.s=.o))
DEPS     := $(OBJS:.o=.d)

# === Default target ===
all: $(ISO)

# === ASM compilation ===
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<

# === C compilation ===
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $<

# === Linking ===
$(ELF): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

# === ISO build ===
$(ISO): $(ELF)
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(ELF) $(ISO_DIR)/boot/
	@cp $(GRUB_DIR)/grub.cfg $(ISO_DIR)/boot/grub/
	grub-mkrescue -o $@ $(ISO_DIR)/ --locales="" --fonts="" --themes="" \
		--modules="multiboot normal iso9660 biosdisk ls" \
		--directory=$(HOME)/local/usr/lib/grub/i386-pc

# === QEMU launch ===
run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -serial stdio -m 2G

# === Clean ===
clean:
	rm -rf $(BUILD_DIR) $(ELF)

fclean: clean
	rm -f $(ISO)
	rm -rf $(ISO_DIR)

re: fclean all

# === Include dependencies ===
-include $(DEPS)

.PHONY: all clean fclean re run


#mkdir -p ~/local
#apt download grub-pc-bin
#dpkg-deb -x grub-pc-bin*.deb ~/local/
#rm -f grub-pc-bin*.deb