#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>

#include "kernel/io.h"
#include "kernel/serial.h"

#define PORT 0x3f8

int init_serial() {
    outportb(PORT + 1, 0x00);    // Disable all interrupts
    outportb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outportb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outportb(PORT + 1, 0x00);    //                  (hi byte)
    outportb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outportb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outportb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    outportb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
    outportb(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
    if (inportb(PORT) != 0xAE) {
        return 1;
    }
    outportb(PORT + 4, 0x0F);    // Test passed, (normal operation mode)
    return 0;
}

void write_serial_char(char a) {
    while (inportb(PORT + 5) & 0x20 == 0);
    outportb(PORT, a);
}

void write_serial_string(const char* a) {
    for (int i = 0; a[i] != '\0'; i++) {
        write_serial_char(a[i]);
    }
}

#define to_hex(c) ((c) < 10 ? '0' + (c) : 'A' + ((c) - 10))

void write_serial_hex(unsigned long num) {
    write_serial_string("0x");

    if (num == 0) {
        write_serial_char('0');
        return;
    }

    bool leading_zero = true;
    for (int i = (sizeof(num) * 8) - 4; i >= 0; i -= 4) {
        int nibble = (num >> i) & 0xF;

        if (nibble != 0 || !leading_zero) {
            write_serial_char(to_hex(nibble));
            leading_zero = false;
        }
    }
}

void write_serial_int(long num) {

    if (num == 0) {
        write_serial_char('0');
        return;
    }

    if (num < 0) {
        write_serial_char('-');
        num = -num;
    }

    char buff[20];
    size_t i = 0;

    while (num > 0) {
        buff[i++] = (num % 10) + '0';
        num /= 10;
    }

    while (i > 0) {
        write_serial_char(buff[--i]);
    }
}

void va_write_serial(const char* format, va_list args) {

    for (size_t i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%' && format[i+1] != '\0') {
            i++;
            switch (format[i]) {
                case 'd':
                    write_serial_int(va_arg(args, int));
                    break;
                case 'x':
                    write_serial_hex(va_arg(args, unsigned int));
                    break;
                case 's':
                    write_serial_string(va_arg(args, const char*));
                    break;
                case 'c':
                    write_serial_char((char)va_arg(args, int));
                    break;
                case '%':
                    write_serial_char('%');
                    break;
                default:
                    write_serial_char('%');
                    write_serial_char(format[i]);
            }
        } else {
            write_serial_char(format[i]);
        }
    }
}
