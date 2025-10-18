#ifndef MULTIBOOT2.H
#define MULTIBOOT2.H

typedef unsigned char           multiboot_uint8_t;
typedef unsigned short          multiboot_uint16_t;
typedef unsigned int            multiboot_uint32_t;
typedef unsigned long long      multiboot_uint64_t;

struct multiboot_tag {
    multiboot_uint32_t type;
    multiboot_uint32_t size;
}__attribute__((packed));

struct  multiboot_tag_framebuffer_common {
    multiboot_uint32_t type;
    multiboot_uint32_t size;

    multiboot_uint64_t framebuffer_addr;
    multiboot_uint32_t framebuffer_pitch;
    multiboot_uint32_t framebuffer_width;
    multiboot_uint32_t framebuffer_height;
    multiboot_uint8_t framebuffer_bpp;
#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED 0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB     1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT     2
    multiboot_uint8_t framebuffer_type;
    multiboot_uint8_t reserved;
}__attribute__((packed));


#endif 
