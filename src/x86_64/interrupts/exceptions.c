#include "interrupts/exceptions.h"
#include "print.h"
#include "kernel/serial.h"
#include <stdarg.h>

void page_fault_handler(struct InterruptStackFrame *frame) {
    unsigned long cr2;
    asm volatile("mov %%cr2, %0" : "=r"(cr2));

    error("Page fault at address: %x", cr2);
}
