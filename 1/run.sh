nasm -felf32 src/multiboot.s -o obj/multiboot.o
i686-elf-gcc -c src/kernel_main.c -o obj/kernel.o -ffreestanding -W -W -W 
i686-elf-gcc -T linker.ld -o iso/boot/os.bin -ffreestanding -O2 -nostdlib obj/kernel.o obj/multiboot.o -lgcc
sudo grub-mkrescue -o k.iso iso/
qemu-system-i386 -cdrom k.iso -boot d -m 2048 -smp 4 -d int
