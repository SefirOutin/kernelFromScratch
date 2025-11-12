# kernelFromScratch

Small Multiboot2 BIOS kernel that boots with GRUB and runs under QEMU.

Quick notes
- Prereqs: nasm, binutils (ld), grub-mkrescue (with i386-pc files), qemu-system-i386.


Make targets (simple)
- make        — assemble & link kernel, put kernel in iso/ and build bootable ISO
- make run    — launch the ISO in QEMU
- make clean  — remove build artifacts