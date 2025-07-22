nasm -f bin -o iso/boot/multiboot.bin src/multiboot_header.s
sudo grub-mkrescue -o k.iso iso/
qemu-system-i386 -cdrom k.iso -boot d -m 2048 -smp 4
