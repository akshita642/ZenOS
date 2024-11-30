#include "arch/x86_64/interrupt/isr.h"
#include "arch/x86_64/interrupt/pic.h"
#include "kernel/kprintf.h"
#include "arch/x86_64/io.h"

char scancode_to_ascii(uint8_t scancode) {
    static char ascii_table[] = {
        0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z',
        'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0
    };
    if (scancode > 0 && scancode < sizeof(ascii_table))
        return ascii_table[scancode];
    return 0;
}

__attribute__((interrupt))
void irq_keyboard_handler(struct InterruptStackFrame* frame) {

    (void) frame;

    uint8_t scan_code = in(0x60);

    char key = scancode_to_ascii(scan_code);

    if (key) {
        kprintf(INFO, "%c", key);
    }

    pic_eoi(0x21);
}
