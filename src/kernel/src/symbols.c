#include "type.h"

struct symbol_entry {
    unsigned int addr;
    const char *name;
};

static struct symbol_entry symbol_table[] = {
    {0x00201000, "keyboard_constructor"},
    {0x00201062, "translate"},
    {0x002010f9, "extended_keys"},
    {0x002011f3, "handle_keyboard_scancode"},
    {0x002012cc, ".L37"},
    {0x0020131e, ".L36"},
    {0x00201338, ".L38"},
    {0x00201359, ".L35"},
    {0x0020137d, ".L34"},
    {0x0020139e, ".L32"},
    {0x002013bf, ".L27"},
    {0x0020141f, "inb"},
    {0x00201446, "isalpha"},
    {0x00201482, "isprint"},
    {0x002014a9, "memcpy"},
    {0x00201509, "memset"},
    {0x00201549, "outb"},
    {0x00201574, "vprintf"},
    {0x002015c4, "printf"},
    {0x0020161b, "check_format"},
    {0x00201698, "oula"},
    {0x0020188d, "pf_putchar"},
    {0x002018bc, "pf_putnbr"},
    {0x0020196a, "pf_putstr"},
    {0x002019ba, "pf_putnbr_base"},
    {0x00201a28, "pf_convert_base"},
    {0x00201a5e, ".L48"},
    {0x00201a7a, ".L46"},
    {0x00201a96, ".L51"},
    {0x00201aaf, ".L49"},
    {0x00201b14, ".L50"},
    {0x00201b2c, ".L44"},
    {0x00201b31, "strchr"},
    {0x00201b80, "strlen"},
    {0x00201bb2, "serial_init"},
    {0x00201c4e, "serial_can_tx"},
    {0x00201c7c, "serial_putchar"},
    {0x00201cc0, "serial_write"},
    {0x00201d1c, "vga_buffer_constructor"},
    {0x00201d85, "set_byte"},
    {0x00201daf, "set_cursor"},
    {0x00201dd4, "get_cursor"},
    {0x00201dfb, "dup"},
    {0x00201e2a, "vga_console_constructor"},
    {0x00201f00, "vga_entry"},
    {0x00201f2c, "vga_putchar"},
    {0x00202062, "vga_delchar"},
    {0x002021c4, "vga_clear"},
    {0x0020222a, "vga_set_color"},
    {0x0020225e, "vga_set_cursor"},
    {0x00202358, "vga_scroll"},
    {0x00202423, "vga_flush"},
    {0x002024a9, "vga_set_buffer"},
    {0x0020256a, "printk"},
    {0x002025c3, "setup_gdt"},
    {0x002026fb, "gdt_set_gate"},
    {0x00202785, "ps2_driver_constructor"},
    {0x002027f0, "has_data_ps2"},
    {0x00202821, "send_byte_ps2"},
    {0x0020287a, "read_byte_ps2"},
    {0x002028bc, "read_byte_ps2_timeout"},
    {0x00202922, "write_ps2_config"},
    {0x00202966, "init_ps2_controller"},
    {0x00202c4d, "test_ps2_port"},
    {0x00202da8, "identify_ps2_device"},
    {0x00202e97, "get_tr"},
    {0x00202eb5, "kinit"},
    {0x00202f14, "putstr"},
    {0x00202f5e, "userHello"},
    {0x00202f82, "kernel"},
    {0x0020310d, "dump_stack"},
    {0x002031f1, "resolve_symbol"},
    {0x00203275, "init_tss"},
    {0x002032d0, "_start"},
    {0x002032dd, "_start.loop"},
    {0x002032e0, "load_gdt"},
    {0x002032fb, "load_tss"},
    {0x00203303, "reload_code_segment"},
    {0x0020330a, "reload_data_segments"},
    {0x00203320, "switch_to_user_mode"},
    {0x0020333d, "__x86.get_pc_thunk.ax"},
    {0x00203341, "__x86.get_pc_thunk.dx"},
    {0x00203345, "__x86.get_pc_thunk.bx"},
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
