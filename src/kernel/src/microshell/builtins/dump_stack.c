#include "type.h"
#include "printk.h"
#include "lib.h"

extern const char *resolve_symbol(unsigned int addr);

void dump_stack(void)
{
    unsigned int *ebp;
    unsigned int *esp;
    unsigned int eip;
    
    asm volatile("movl %%ebp, %0" : "=r"(ebp));
    asm volatile("movl %%esp, %0" : "=r"(esp));
    
    printk(LOG_INFO, "=== KERNEL STACK DUMP ===\n");
    printk(LOG_INFO, "Current ESP: 0x%x\n", esp);
    printk(LOG_INFO, "Current EBP: 0x%x\n", ebp);
    printk(LOG_INFO, "Call trace:\n");
    
    int frame = 0;
    while (ebp != 0 && frame < 10)
    {
        eip = ebp[1];
        const char *symbol = resolve_symbol(eip);
        printk(LOG_INFO, " [%d] 0x%x <%s>\n", frame, eip, symbol);
        
        ebp = (unsigned int *)ebp[0];
        frame++;
    }
    
    printk(LOG_INFO, "=========================\n");
}

void dump_stack_builtin(void)
{
    putstr("KERNEL STACK DUMPED TO SERIAL\n");
    dump_stack();
}
