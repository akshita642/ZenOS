#include "arch/x86_64/interrupt/idt.h"
#include "arch/x86_64/interrupt/isr.h"
#include "arch/x86_64/interrupt/pic.h"
#include "arch/x86_64/asm.h"
#include "kernel/kprintf.h"
#include "drivers/keyboard.h"

#define IDT_SIZE 256

struct IDT_Entry idt[IDT_SIZE];
struct IDT_Ptr idtp;

void setup_isr() {
    set_idt_entry(&idt[0], (uint64_t) isr_divide_error, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[1], (uint64_t) isr_debug, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[2], (uint64_t) isr_non_maskable_interrupt, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[3], (uint64_t) isr_breakpoint, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[4], (uint64_t) isr_overflow, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[5], (uint64_t) isr_bound_range_exceeded, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[6], (uint64_t) isr_inavlid_opcode, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[7], (uint64_t) isr_device_not_found, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[8], (uint64_t) isr_double_fault, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[9], (uint64_t) isr_coprocess_segment_overrun, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[10], (uint64_t) isr_invalid_tss, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[11], (uint64_t) isr_segment_not_present, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[12], (uint64_t) isr_stack_segment_fault, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[13], (uint64_t) isr_general_protection_fault, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[14], (uint64_t) isr_page_fault, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[15], (uint64_t) isr_reserved, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[16], (uint64_t) isr_x87_floating_point_exception, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[17], (uint64_t) isr_alignment_check, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[18], (uint64_t) isr_machine_check, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[19], (uint64_t) isr_simd_floating_point_exception, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[20], (uint64_t) isr_virtualization_exception, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[21], (uint64_t) isr_control_protection_exception, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[22], (uint64_t) isr_reserved1, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[23], (uint64_t) isr_reserved2, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[24], (uint64_t) isr_reserved3, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[25], (uint64_t) isr_reserved4, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[26], (uint64_t) isr_reserved5, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[27], (uint64_t) isr_reserved6, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[28], (uint64_t) isr_reserved7, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[29], (uint64_t) isr_hypervisor_injection_exception, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[30], (uint64_t) isr_vmm_communication_exception, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    set_idt_entry(&idt[31], (uint64_t) isr_security_exception, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);

    set_idt_entry(&idt[33], (uint64_t) irq_keyboard_handler, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
}

void init_idt() {

    kprintf(DEBUG, "Initialize IDT........................................");
    
    idtp.size = (sizeof(struct IDT_Entry) * IDT_SIZE) - 1;
    idtp.offset = (uint64_t) &idt;

    for (int i = 0; i < IDT_SIZE; ++i) {
        set_idt_entry(&idt[i], (uint64_t) isr_default_handler, 0x08, 1, IDT_ENTRY_KERNEL, IDT_TYPE_INTERRUPT);
    }

    setup_isr();

    lidt((uint64_t) &idtp);

    kprintf(INFO, "[Success]\n");

    kprintf(DEBUG, "Remap PIC.............................................");

    init_pic(0x20, 0x28);
    irq_clear_mask(1);
    sti();

    kprintf(INFO, "[Success]\n");
}

void set_idt_entry(struct IDT_Entry* entry, uint64_t handler, uint16_t selector, uint8_t ist, uint8_t flags, uint8_t gate_type) {
    entry->offset_low = handler & 0xFFFF;
    entry->selector = selector;
    entry->ist = ist & 0x3;
    entry->type_attr = flags | gate_type;
    entry->offset_mid = (handler >> 16) & 0xFFFF;
    entry->offset_high = (handler >> 32);
    entry->reserved = 0;
}
