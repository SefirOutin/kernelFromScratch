AS = nasm
CC = gcc
LD = ld
ASFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -fno-builtin -fno-stack-protector -nostdlib -c
LDFLAGS = -m elf_i386 -T linker.ld
ISO = kernel.iso

# 🔹 Cible par défaut : tout construire
all: $(ISO)

multiboot_header.o: src/boot/multiboot_header.s
	$(AS) $(ASFLAGS) src/boot/multiboot_header.s -o src/boot/multiboot_header.o

main.o: src/kernel/main.c
	$(CC) $(CFLAGS) src/kernel/main.c -o src/kernel/main.o

kernel.elf: src/boot/multiboot_header.o src/kernel/main.o
	$(LD) $(LDFLAGS) src/boot/multiboot_header.o src/kernel/main.o -o kernel.elf

# Vérifie que le dossier i386-pc existe, sinon le télécharge et l'extrait localement
$(HOME)/local/usr/lib/grub/i386-pc:
	@echo "⚙️  Dossier i386-pc manquant, téléchargement du grub-pc-bin..."
	mkdir -p $(HOME)/local/usr/lib/grub/
	wget -q http://ftp.fr.debian.org/debian/pool/main/g/grub2/grub-pc-bin_2.12-1_i386.deb -O /tmp/grub-pc-bin.deb
	dpkg-deb -x /tmp/grub-pc-bin.deb $(HOME)/local/
	rm -f /tmp/grub-pc-bin.deb
	@echo "✅ Modules GRUB installés localement dans $(HOME)/local/usr/lib/grub/i386-pc"

$(ISO): $(HOME)/local/usr/lib/grub/i386-pc kernel.elf
	mkdir -p iso/boot/grub/
	cp kernel.elf iso/boot/
	cp grub/grub.cfg iso/boot/grub/
	grub-mkrescue -o $(ISO) iso/ --locales="" --fonts="" --themes="" \
		--modules="multiboot normal iso9660 biosdisk ls" \
		--directory=$(HOME)/local/usr/lib/grub/i386-pc

# 🔹 Nettoyage basique
clean:
	rm -f src/boot/*.o src/kernel/*.o kernel.elf

# 🔹 Nettoyage complet
fclean: clean
	rm -f $(ISO)
	rm -rf iso

# 🔹 Rebuild complet
re: fclean all

run: kernel.iso
	qemu-system-i386 -cdrom $(ISO)