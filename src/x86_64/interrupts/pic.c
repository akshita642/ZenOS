#include "kernel/io.h"
#include "interrupts/pic.h"

void pic_remap() {
    outportb(PIC1_COMMAND, 0x11); // ICW1: Start initialization
    outportb(PIC2_COMMAND, 0x11);
    outportb(PIC1_DATA, 0x20); // ICW2: Master PIC interrupt vector base (0x20)
    outportb(PIC2_DATA, 0x28); // ICW2: Slave PIC interrupt vector base (0x28)
    outportb(PIC1_DATA, 0x04); // ICW3: Slave PIC is connected to IRQ2
    outportb(PIC2_DATA, 0x02); // ICW3: Slave PIC is connected to IRQ2
    outportb(PIC1_DATA, 0x01); // ICW4: 8086 mode
    outportb(PIC2_DATA, 0x01); // ICW4: 8086 mode

    // Mask all interrupts (disable them)
    outportb(PIC1_DATA, 0xFF); // Disable all IRQs on master PIC
    outportb(PIC2_DATA, 0xFF); // Disable all IRQs on slave PIC
}

void enable_irq() {
    outportb(PIC1_DATA, 0xFD); // Enable IRQ1
    outportb(PIC2_DATA, 0xFF); // Disable all IRQs on the slave
}
