#include "type.h"

struct symbol_entry {
    unsigned int addr;
    const char *name;
};

static struct symbol_entry symbol_table[] = {
    {0x00201000, "setup_gdt"},
    {0x00201138, "gdt_set_gate"},
    {0x002011c2, "get_tr"},
    {0x002011e0, "init_tss"},
    {0x00201228, "STUM"},
    {0x00201263, "kinit"},
    {0x002012c2, "userHello"},
    {0x002012d2, "aligned_addr"},
    {0x002012f3, "kernel"},
    {0x00201399, "resolve_symbol"},
    {0x0020141d, "printk"},
    {0x00201476, "vga_buffer_constructor"},
    {0x002014df, "set_byte"},
    {0x00201509, "set_cursor"},
    {0x0020152e, "get_cursor"},
    {0x00201555, "dup"},
    {0x00201584, "vga_console_constructor"},
    {0x0020165a, "vga_entry"},
    {0x00201686, "vga_putchar"},
    {0x002017bc, "vga_delchar"},
    {0x0020191e, "vga_clear"},
    {0x00201984, "vga_set_color"},
    {0x002019b8, "vga_set_cursor"},
    {0x00201ab2, "vga_scroll"},
    {0x00201b7d, "vga_flush"},
    {0x00201c03, "vga_set_buffer"},
    {0x00201cc4, "serial_init"},
    {0x00201d60, "serial_can_tx"},
    {0x00201d8e, "serial_putchar"},
    {0x00201dd2, "serial_write"},
    {0x00201e2e, "keyboard_constructor"},
    {0x00201e90, "translate"},
    {0x00201f27, "extended_keys"},
    {0x00201fd9, "handle_keyboard_scancode"},
    {0x002020b2, ".L32"},
    {0x00202104, ".L31"},
    {0x0020211e, ".L33"},
    {0x0020213f, ".L30"},
    {0x00202163, ".L29"},
    {0x00202184, ".L27"},
    {0x002021a5, ".L22"},
    {0x00202205, "ps2_driver_constructor"},
    {0x00202270, "has_data_ps2"},
    {0x002022a1, "send_byte_ps2"},
    {0x002022fa, "read_byte_ps2"},
    {0x0020233c, "read_byte_ps2_timeout"},
    {0x002023a2, "send_cmd_ps2"},
    {0x002023ea, "write_ps2_config"},
    {0x00202426, "init_ps2_controller"},
    {0x002026fd, "test_ps2_port"},
    {0x00202850, "identify_ps2_device"},
    {0x0020293d, "aligned_jmp"},
    {0x0020295a, "parse_mmap"},
    {0x00202a2e, "parse_boot_struct"},
    {0x00202ab5, "print_mmap"},
    {0x00202bbc, "readline"},
    {0x00202ca1, "decode_cmd"},
    {0x00202dbf, "microshell"},
    {0x00202e66, "reboot"},
    {0x00202e90, "clear"},
    {0x00202ebb, "halt"},
    {0x00202ecc, "dump_stack"},
    {0x00202fb0, "dump_stack_builtin"},
    {0x00202fe0, "vprintf"},
    {0x00203030, "printf"},
    {0x00203087, "check_format"},
    {0x00203104, "oula"},
    {0x002032f7, "pf_putchar"},
    {0x00203326, "pf_putnbr"},
    {0x002033cc, "pf_putstr"},
    {0x0020341c, "pf_putnbr_base"},
    {0x0020348a, "pf_convert_base"},
    {0x002034c0, ".L48"},
    {0x002034dc, ".L46"},
    {0x002034f8, ".L51"},
    {0x00203511, ".L49"},
    {0x00203572, ".L50"},
    {0x0020358a, ".L44"},
    {0x0020358f, "memset"},
    {0x002035cf, "isprint"},
    {0x002035f6, "putchar"},
    {0x00203661, "strcmp"},
    {0x002036df, "memcpy"},
    {0x0020373f, "strchr"},
    {0x0020378e, "strncmp"},
    {0x00203814, "inb"},
    {0x0020383b, "isalpha"},
    {0x00203877, "outb"},
    {0x002038a0, "putstr"},
    {0x00203920, "strlen"},
    {0x00203960, "_start"},
    {0x0020396d, "_start.loop"},
    {0x00203970, "load_gdt"},
    {0x0020398b, "load_tss"},
    {0x00203993, "reload_code_segment"},
    {0x0020399a, "reload_data_segments"},
    {0x002039b0, "switch_to_user_mode"},
    {0x002039cd, "__x86.get_pc_thunk.ax"},
    {0x002039d1, "__x86.get_pc_thunk.bx"},
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
