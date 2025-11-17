#include "symbols.h"

#define NULL ((void*)0)

// Table des symboles - sera générée automatiquement
struct symbol_entry {
    unsigned int addr;
    const char *name;
};

// Déclarer les adresses des fonctions comme symboles externes
extern void kernel(void);
extern void kinit(void);
extern void setup_gdt(void);
extern void init_tss(void);
extern void dump_stack(void);
extern void switch_to_user_mode(void);
extern void userHello(void);
extern void serial_init(void);
extern void ps2_driver_constructor(void);
extern void vga_console_constructor(void);

// Table des symboles manuelle (en attendant la génération automatique)
static struct symbol_entry symbol_table[] = {
    {(unsigned int)kernel, "kernel"},
    {(unsigned int)kinit, "kinit"},
    {(unsigned int)setup_gdt, "setup_gdt"},
    {(unsigned int)init_tss, "init_tss"},
    {(unsigned int)dump_stack, "dump_stack"},
    {(unsigned int)switch_to_user_mode, "switch_to_user_mode"},
    {(unsigned int)userHello, "userHello"},
    {(unsigned int)serial_init, "serial_init"},
    {(unsigned int)ps2_driver_constructor, "ps2_driver_constructor"},
    {(unsigned int)vga_console_constructor, "vga_console_constructor"},
    {0, NULL}
};

const char* resolve_symbol(unsigned int addr)
{
    int best_match = -1;
    unsigned int best_addr = 0;
    
    // Chercher le symbole le plus proche (inférieur ou égal à addr)
    for (int i = 0; symbol_table[i].name != NULL; i++)
    {
        if (symbol_table[i].addr <= addr && symbol_table[i].addr > best_addr)
        {
            best_match = i;
            best_addr = symbol_table[i].addr;
        }
    }
    
    if (best_match >= 0)
        return symbol_table[best_match].name;
    
    return "??";
}
