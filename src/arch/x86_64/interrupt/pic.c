#include "arch/x86_64/interrupt/pic.h"
#include "arch/x86_64/io.h"
#include "kernel/kprintf.h"

void init_pic(uint8_t offset1, uint8_t offset2) {

    out(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    out(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    out(PIC1_DATA, offset1);
    io_wait();
    out(PIC2_DATA, offset2);
    io_wait();

    out(PIC1_DATA, 4);  // Slave connected to IRQ2
    io_wait();
    out(PIC2_DATA, 2);  // Tell Slave PIC its cascade identity
    io_wait();

    out(PIC1_DATA, ICW4_8086);  // Use 8086 mode (and not 8088 mode)
    io_wait();
    out(PIC2_DATA, ICW4_8086);
    io_wait();

    out(PIC1_DATA, 0xFF);   // mask all interrupts
    out(PIC2_DATA, 0xFF);
}

void pic_eoi(uint8_t irq) {
    if (irq >= 8) {
        out(PIC2_COMMAND, PIC_EOI);
    }
    out(PIC1_COMMAND, PIC_EOI);
}

void pic_disable() {
    out(PIC1_DATA, 0xFF);
    out(PIC2_DATA, 0xFF);
}

void irq_set_mask(uint8_t irq) {

    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
    }

    value = in(port) | (1 << irq);
    out(port, value);
}

void irq_clear_mask(uint8_t irq) {

    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
    }

    value = in(port) & ~(1 << irq);
    out(port, value);
}
