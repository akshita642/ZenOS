#include "arch/x86_64/interrupt/isr.h"
#include "kernel/kprintf.h"

void print_interrupt_stack_frame(struct InterruptStackFrame* frame) {

    kprintf(ERROR, "RIP: %d\n", frame->rip);
    kprintf(ERROR, "CS: %d\n", frame->cs);
    kprintf(ERROR, "RFLAGS: %d\n", frame->rflags);
    kprintf(ERROR, "RSP: %d\n", frame->rsp);
    kprintf(ERROR, "SS: %d\n", frame->ss);
}

__attribute__((interrupt))
void isr_default_handler(struct InterruptStackFrame* frame) {

    (void) frame;

    kprintf(ERROR, "Unhandled interrupt called\n");
    asm("cli; hlt");
    while (1);
}

void default_handler(struct InterruptStackFrame* frame, uint64_t error_code) {
    if (error_code != 0) {
        kprintf(ERROR, "Error Code: %d\n", error_code);
    }

    print_interrupt_stack_frame(frame);

    asm("cli; hlt");
    while(1);
}

#define define_exception(isr_func) \
    __attribute__((interrupt)) \
    void isr_func(struct InterruptStackFrame* frame, uint64_t error_code) { \
        kprintf(ERROR, "%s\n", __func__); \
        default_handler(frame, error_code); \
    }

#define define_exception_no_code(isr_func) \
    __attribute__((interrupt)) \
    void isr_func(struct InterruptStackFrame* frame) { \
        kprintf(ERROR, "%s\n", __func__); \
        default_handler(frame, 0); \
    }

define_exception_no_code(isr_divide_error)
define_exception_no_code(isr_debug)
define_exception_no_code(isr_non_maskable_interrupt)
define_exception_no_code(isr_breakpoint)
define_exception_no_code(isr_overflow)
define_exception_no_code(isr_bound_range_exceeded)
define_exception_no_code(isr_inavlid_opcode)
define_exception_no_code(isr_device_not_found)
define_exception(isr_double_fault)
define_exception_no_code(isr_coprocess_segment_overrun)
define_exception(isr_invalid_tss)
define_exception(isr_segment_not_present)
define_exception(isr_stack_segment_fault)
define_exception(isr_general_protection_fault)
define_exception(isr_page_fault)
define_exception_no_code(isr_reserved)
define_exception_no_code(isr_x87_floating_point_exception)
define_exception(isr_alignment_check)
define_exception_no_code(isr_machine_check)
define_exception_no_code(isr_simd_floating_point_exception)
define_exception_no_code(isr_virtualization_exception)
define_exception(isr_control_protection_exception)
define_exception_no_code(isr_reserved1);
define_exception_no_code(isr_reserved2);
define_exception_no_code(isr_reserved3);
define_exception_no_code(isr_reserved4);
define_exception_no_code(isr_reserved5);
define_exception_no_code(isr_reserved6);
define_exception_no_code(isr_reserved7);
define_exception(isr_hypervisor_injection_exception)
define_exception(isr_vmm_communication_exception)
define_exception_no_code(isr_security_exception)
