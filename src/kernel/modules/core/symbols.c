#include "type.h"

struct symbol_entry {
    unsigned int addr;
    const char *name;
};

static struct symbol_entry symbol_table[] = {
    {0x00201000, "setup_gdt"},
    {0x00201138, "gdt_set_gate"},
    {0x002011c2, "get_tr"},
    {0x002011e0, "kinit"},
    {0x0020123f, "userHello"},
    {0x00201266, "kernel"},
    {0x002012eb, "keyboard_constructor"},
    {0x0020134d, "translate"},
    {0x002013e4, "extended_keys"},
    {0x002014b8, "handle_keyboard_scancode"},
    {0x00201591, ".L34"},
    {0x002015e3, ".L33"},
    {0x002015fd, ".L35"},
    {0x0020161e, ".L32"},
    {0x00201642, ".L31"},
    {0x00201663, ".L29"},
    {0x00201684, ".L24"},
    {0x002016e4, "inb"},
    {0x0020170b, "isalpha"},
    {0x00201747, "isprint"},
    {0x0020176e, "memcpy"},
    {0x002017ce, "memset"},
    {0x0020180e, "outb"},
    {0x00201839, "vprintf"},
    {0x00201889, "printf"},
    {0x002018e0, "check_format"},
    {0x0020195d, "oula"},
    {0x00201b52, "pf_putchar"},
    {0x00201b81, "pf_putnbr"},
    {0x00201c2f, "pf_putstr"},
    {0x00201c7f, "pf_putnbr_base"},
    {0x00201ced, "pf_convert_base"},
    {0x00201d23, ".L48"},
    {0x00201d3f, ".L46"},
    {0x00201d5b, ".L51"},
    {0x00201d74, ".L49"},
    {0x00201dd9, ".L50"},
    {0x00201df1, ".L44"},
    {0x00201df6, "putstr"},
    {0x00201e43, "strchr"},
    {0x00201e92, "strlen"},
    {0x00201ec4, "strncmp"},
    {0x00201f48, "strcmp"},
    {0x00201fc0, "printk"},
    {0x00202019, "ps2_driver_constructor"},
    {0x00202084, "has_data_ps2"},
    {0x002020b5, "send_byte_ps2"},
    {0x0020210e, "read_byte_ps2"},
    {0x00202150, "read_byte_ps2_timeout"},
    {0x002021b6, "write_ps2_config"},
    {0x002021fa, "init_ps2_controller"},
    {0x002024e1, "test_ps2_port"},
    {0x0020263c, "identify_ps2_device"},
    {0x0020272b, "serial_init"},
    {0x002027c7, "serial_can_tx"},
    {0x002027f5, "serial_putchar"},
    {0x00202839, "serial_write"},
    {0x00202895, "resolve_symbol"},
    {0x00202919, "init_tss"},
    {0x00202961, "vga_buffer_constructor"},
    {0x002029ca, "set_byte"},
    {0x002029f4, "set_cursor"},
    {0x00202a19, "get_cursor"},
    {0x00202a40, "dup"},
    {0x00202a6f, "vga_console_constructor"},
    {0x00202b45, "vga_entry"},
    {0x00202b71, "vga_putchar"},
    {0x00202ca7, "vga_delchar"},
    {0x00202e09, "vga_clear"},
    {0x00202e6f, "vga_set_color"},
    {0x00202ea3, "vga_set_cursor"},
    {0x00202f9d, "vga_scroll"},
    {0x00203068, "vga_flush"},
    {0x002030ee, "vga_set_buffer"},
    {0x002031af, "readline"},
    {0x00203319, "decode_cmd"},
    {0x002033b7, "microshell"},
    {0x0020342c, "dump_stack"},
    {0x00203510, "_start"},
    {0x0020351d, "_start.loop"},
    {0x00203520, "load_gdt"},
    {0x0020353b, "load_tss"},
    {0x00203543, "reload_code_segment"},
    {0x0020354a, "reload_data_segments"},
    {0x00203560, "switch_to_user_mode"},
    {0x0020357d, "__x86.get_pc_thunk.ax"},
    {0x00203581, "__x86.get_pc_thunk.bx"},
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
