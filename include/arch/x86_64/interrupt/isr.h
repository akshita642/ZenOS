#pragma once

#include "stdint.h"

struct InterruptStackFrame {
	uint64_t rip, cs, rflags, rsp, ss;
} __attribute__((packed));

__attribute__((interrupt))
void isr_default_handler(struct InterruptStackFrame* frame);

#define declare_exception(isr_func) \
    __attribute__((interrupt)) \
    void isr_func(struct InterruptStackFrame*, uint64_t); 

#define declare_exception_no_code(isr_func) \
    __attribute__((interrupt)) \
    void isr_func(struct InterruptStackFrame*); 

declare_exception_no_code(isr_divide_error)
declare_exception_no_code(isr_debug)
declare_exception_no_code(isr_non_maskable_interrupt)
declare_exception_no_code(isr_breakpoint)
declare_exception_no_code(isr_overflow)
declare_exception_no_code(isr_bound_range_exceeded)
declare_exception_no_code(isr_inavlid_opcode)
declare_exception_no_code(isr_device_not_found)
declare_exception(isr_double_fault)
declare_exception_no_code(isr_coprocess_segment_overrun)
declare_exception(isr_invalid_tss)
declare_exception(isr_segment_not_present)
declare_exception(isr_stack_segment_fault)
declare_exception(isr_general_protection_fault)
declare_exception(isr_page_fault)
declare_exception_no_code(isr_reserved)
declare_exception_no_code(isr_x87_floating_point_exception)
declare_exception(isr_alignment_check)
declare_exception_no_code(isr_machine_check)
declare_exception_no_code(isr_simd_floating_point_exception)
declare_exception_no_code(isr_virtualization_exception)
declare_exception(isr_control_protection_exception)
declare_exception_no_code(isr_reserved1);
declare_exception_no_code(isr_reserved2);
declare_exception_no_code(isr_reserved3);
declare_exception_no_code(isr_reserved4);
declare_exception_no_code(isr_reserved5);
declare_exception_no_code(isr_reserved6);
declare_exception_no_code(isr_reserved7);
declare_exception(isr_hypervisor_injection_exception)
declare_exception(isr_vmm_communication_exception)
declare_exception_no_code(isr_security_exception)
