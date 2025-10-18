#include "multiboot2.h"


void kkernel(unsigned long magic, unsigned long addr)
{

struct multiboot_tag *tag;
  unsigned size;
    char *vga = (char *)0xB8000;  // adresse mémoire VGA texte
    vga[0] = '4';                 // premier caractère
    vga[1] = 0x07;                // couleur gris clair sur fond noir
    vga[2] = '2';                 // second caractère
    vga[3] = 0x07;                // même couleur
    while (1)                     // boucle infinie pour garder l’écran affiché
        ;
}
