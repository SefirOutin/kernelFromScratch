#include "kernel.h"



int kernel_main(uint32_t *s)
{
    t_multiboot_struct mb2Sruct;
    // struct multiboot_tag *tag;
	serialInit();
    memset(&mb2Sruct, 0, sizeof(t_multiboot_struct));
    // printf("\n%d\n", printf("oui %s", "non"));
    // uint32_t structLenght = *s;
    printf("struct len: %dbytes", sizeof(t_multiboot_struct));
    // *((uint32_t *)0xA000) = 1;
    // // uint8_t *tmp = (uint8_t *)s;
    // for (tag = s + 8;
    //     tag->type != MULTIBOOT_TAG_TYPE_END;
    //     tag = (struct multiboot_tag *) ((uint8_t *) tag + ((tag->size + 7) & ~7)))
    // {
        
    // }
    
    
    
    while (1);
}
