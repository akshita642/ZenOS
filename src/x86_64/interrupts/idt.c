#include "interrupts/idt.h"
#include "interrupts/isr.h"
#include "interrupts/exceptions.h"
#include "interrupts/pic.h"
#include "kernel/io.h"
#include "kernel/serial.h"
#include "print.h"
#include "const.h"

#define IDT_ENTRIES 256
struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idtp;

void (*irq_routines[16])(struct InterruptStackFrame *) = {};
void (*exception_handlers[32])(struct InterruptStackFrame *) = {};

void irq_set_routine(uint8_t irq_number, void (*routine)(struct InterruptStackFrame *)) {
	irq_routines[irq_number] = routine;
}

void exception_set_handler(uint8_t exception_number, void (*handler)(struct InterruptStackFrame *)) {
    exception_handlers[exception_number] = handler;
}

extern void keyboard_handler();

void pic_acknowledge(uint8_t irq) {
	if (irq >= 8)
		outportb(PIC2_COMMAND, PIC_EOI);
	outportb(PIC1_COMMAND, PIC_EOI);
}

__attribute__((noreturn))
void handle_exception(struct InterruptStackFrame *frame) {

	error("Exception %d: %s\n", frame->interrupt_num, EXCEPTIONS[frame->interrupt_num]);
    error("code %d\n", frame->error_code);

    error("Interrupt frame at %x \n", (unsigned long)frame);
    error("Instruction pointer: %x \n", frame->rip);
    error("Code segment: %x \n", frame->cs);
    error("CPU flags: %x \n", frame->rflags);
    error("Stack pointer: %x \n", frame->rsp);
    error("Stack segment: %x \n", frame->ss);

	void (*handler)(struct InterruptStackFrame *) = exception_handlers[frame->interrupt_num];
	if (handler)
		handler(frame);
	asm("cli; hlt");
	while (1);
}

void handle_interrupt(struct InterruptStackFrame *frame) {
    uint32_t irq_number = frame->interrupt_num - 32;
    void (*routine)(struct InterruptStackFrame*) = irq_routines[irq_number];
    if (routine)
        routine(frame);
    pic_acknowledge(irq_number);
}

void idt_set_entry(int n, uint64_t handler) {
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector = 0x08;
    idt[n].ist = 0;
    idt[n].type_attr = 0x8E;
    idt[n].offset_mid = (handler >> 16) & 0xFFFF;
    idt[n].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[n].zero = 0;
}

void idt_init() {
    idtp.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
    idtp.base = (uint64_t)&idt;

    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_entry(i, (uint64_t)isr_default_handler);
    }

    idt_set_entry(0, (uint64_t) exception0);
    idt_set_entry(1, (uint64_t) exception1);
    idt_set_entry(2, (uint64_t) exception2);
    idt_set_entry(3, (uint64_t) exception3);
    idt_set_entry(4, (uint64_t) exception4);
    idt_set_entry(5, (uint64_t) exception5);
    idt_set_entry(6, (uint64_t) exception6);
    idt_set_entry(7, (uint64_t) exception7);
    idt_set_entry(8, (uint64_t) exception8);
    idt_set_entry(9, (uint64_t) exception9);
    idt_set_entry(10, (uint64_t) exception10);
    idt_set_entry(11, (uint64_t) exception11);
    idt_set_entry(12, (uint64_t) exception12);
    idt_set_entry(13, (uint64_t) exception13);
    idt_set_entry(14, (uint64_t) exception14);
    idt_set_entry(15, (uint64_t) exception15);
    idt_set_entry(16, (uint64_t) exception16);
    idt_set_entry(17, (uint64_t) exception17);
    idt_set_entry(18, (uint64_t) exception18);
    idt_set_entry(19, (uint64_t) exception19);
    idt_set_entry(20, (uint64_t) exception20);
    idt_set_entry(21, (uint64_t) exception21);
    idt_set_entry(22, (uint64_t) exception22);
    idt_set_entry(23, (uint64_t) exception23);
    idt_set_entry(24, (uint64_t) exception24);
    idt_set_entry(25, (uint64_t) exception25);
    idt_set_entry(26, (uint64_t) exception26);
    idt_set_entry(27, (uint64_t) exception27);
    idt_set_entry(28, (uint64_t) exception28);
    idt_set_entry(29, (uint64_t) exception29);
    idt_set_entry(30, (uint64_t) exception30);
    idt_set_entry(31, (uint64_t) exception31);

    idt_set_entry(32, (uint64_t) irq0);
    idt_set_entry(33, (uint64_t) irq1);
    idt_set_entry(34, (uint64_t) irq2);
    idt_set_entry(35, (uint64_t) irq3);
    idt_set_entry(36, (uint64_t) irq4);
    idt_set_entry(37, (uint64_t) irq5);
    idt_set_entry(38, (uint64_t) irq6);
    idt_set_entry(39, (uint64_t) irq7);
    idt_set_entry(40, (uint64_t) irq8);
    idt_set_entry(41, (uint64_t) irq9);
    idt_set_entry(42, (uint64_t) irq10);
    idt_set_entry(43, (uint64_t) irq11);
    idt_set_entry(44, (uint64_t) irq12);
    idt_set_entry(45, (uint64_t) irq13);
    idt_set_entry(46, (uint64_t) irq14);
    idt_set_entry(47, (uint64_t) irq15);

    pic_remap();
    enable_irq();

    exception_set_handler(14, page_fault_handler);

    irq_set_routine(1, keyboard_handler);

    load_idt((uint64_t)&idtp);
}

