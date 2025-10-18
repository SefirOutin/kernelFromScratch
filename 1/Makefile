AS = nasm
CC = gcc
LD = ld
ASFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -fno-builtin -fno-stack-protector -nostdlib -c
LDFLAGS = -m elf_i386 -T linker.ld
ISO = kernel.iso

# 🔹 Cible par défaut : tout construire
all: $(ISO)

boot.o: src/boot/boot.s
	$(AS) $(ASFLAGS) src/boot/boot.s -o src/boot/boot.o

kernel.o: src/kernel/kernel.c
	$(CC) $(CFLAGS) src/kernel/kernel.c -o src/kernel/kernel.o

kernel.elf: src/boot/boot.o src/kernel/kernel.o
	$(LD) $(LDFLAGS) src/boot/boot.o src/kernel/kernel.o -o kernel.elf

# Vérifie que le dossier i386-pc existe, sinon le télécharge et l'extrait localement

$(ISO): kernel.elf
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

#mkdir -p ~/local
#apt download grub-pc-bin
#dpkg-deb -x grub-pc-bin*.deb ~/local/
#rm -f grub-pc-bin*.deb