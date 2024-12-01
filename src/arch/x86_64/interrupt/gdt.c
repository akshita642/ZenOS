#include "arch/x86_64/interrupt/gdt.h"
#include "arch/x86_64/asm.h"
#include "kernel/kprintf.h"
#include "drivers/vga.h"

extern struct GDT_Entry8 gdt64[GDT_SIZE];
extern struct TSS_Segment tss_segment;

#define IST1_STACK_SIZE 4096

uint8_t ist1_stack[IST1_STACK_SIZE];

void init_gdt_with_tss() {

    kprintf(DEBUG, "Initialize GDT........................................");

    struct GDT_Entry16* tss = (struct GDT_Entry16*) &gdt64[3];

    set_gdt_entry8(&gdt64[0], 0, 0, 0, 0);
    set_gdt_entry8(&gdt64[1], 0, (uint16_t) 0xFFFFF, GDT_ENTRY_FLAGS_KERNEL_CODE, GDT_ENTRY_ACCESS_KERNEL_CODE);
    set_gdt_entry8(&gdt64[2], 0, (uint16_t) 0xFFFFF, GDT_ENTRY_FLAGS_KERNEL_DATA, GDT_ENTRY_ACCESS_KERNEL_DATA);

    set_gdt_entry16(tss, (uint64_t) &tss_segment, sizeof(struct TSS_Segment) - 1, GDT_ENTRY_ACCESS_TSS, GDT_ENTRY_FLAGS_TSS);

    kprintf(INFO, "[Success]\n");

    kprintf(DEBUG, "Load TSS Segment......................................");

    init_tss_segment(&tss_segment);
    
    lgdt();
    ltr();

    kprintf(INFO, "[Success]\n");
}

void set_gdt_entry8(struct GDT_Entry8* entry, uint32_t base_address, uint16_t size, uint8_t flags, uint8_t access) {

    entry->limit_low = size & 0xFFFF;
    entry->base_low = base_address & 0xFFFF;
    entry->base_mid = (base_address >> 16) & 0xFF;
    entry->access = access;
    entry->limit_high_and_flags = ((size >> 16) & 0x0F) | flags;
    entry->base_high = (base_address >> 24) & 0xFF;
}

void set_gdt_entry16(struct GDT_Entry16* entry, uint64_t base_address, uint32_t size, uint8_t access, uint8_t flags) {

    entry->limit_low = size & 0xFFFF;
    entry->base_low = base_address & 0xFFFF;
    entry->base_mid = (base_address >> 16) & 0xFF;
    entry->access = access;
    entry->limit_high_and_flags = ((size >> 16) & 0x0F) | flags;
    entry->base_mid_upper = (base_address >> 24) & 0xFF;
    entry->base_high = (base_address >> 32);
    entry->reserved = 0;
}

void init_tss_segment(struct TSS_Segment* tss_segment) {
    tss_segment->ist[0] = (uint64_t) &ist1_stack + IST1_STACK_SIZE;
    tss_segment->iomap_base_address = 0xFFFF;
}
