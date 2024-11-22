#include "print.h"
#include "kernel/serial.h"
#include "kernel/io.h"
#include "interrupts/idt.h"
#include "multiboot2/multiboot2.h"

void kernel_main(unsigned long addr) {

    print_clear();
    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
    
    kprintf("Welcome to ZenOS Kernel\n");

    init_serial();

    idt_init();
    sti();
}
