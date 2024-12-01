#include "drivers/vga.h"
#include "arch/x86_64/io.h"

#define FB_COMMAND_PORT     0x3D4
#define FB_DATA_PORT        0X3D5

#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

static const size_t NUM_COLS = 80;
static const size_t NUM_ROWS = 25;

struct Char {
    uint8_t character;
    uint8_t color;
};

struct Char volatile* buffer = (struct Char*) 0xB8000;

size_t col = 0;
size_t row = 0;
uint8_t color = PRINT_COLOR_WHITE | PRINT_COLOR_BLACK << 4;

void clear_row(size_t row) {
    struct Char empty = (struct Char) {
        .character = ' ',
        .color = color,
    };
    for (size_t col = 0; col < NUM_COLS; ++col) {
        buffer[col + NUM_COLS * row] = empty;
    }
}

void vga_clear_screen() {
    for (size_t i = 0; i < NUM_ROWS; ++i) {
        clear_row(i);
    }
    vga_set_cursor(0, 0);
    vga_update_cursor();
}

void vga_set_color(uint8_t foreground, uint8_t background) {
    color = foreground + (background << 4);
}

void print_newline() {
    col = 0;

    if (row < NUM_ROWS - 1) {
        row++;
        return;
    }

    for (size_t row = 1; row < NUM_ROWS; row++) {
        for (size_t col = 0; col < NUM_COLS; col++) {
            struct Char character = buffer[col + NUM_COLS * row];
            buffer[col + NUM_COLS * (row - 1)] = character;
        }
    }

    clear_row(NUM_COLS - 1);

    vga_update_cursor();
}

void backspace() {
    if (col == 0) {
        if (row != 0) {
            col = NUM_COLS - 1;
            row = row - 1;
        }
    } else {
        col = col - 1;
    }

    struct Char empty = (struct Char) {
        .character = ' ',
        .color = color,
    };

    buffer[col + NUM_COLS * row] = empty;
}

void vga_write_char(char character) {
    if (character == '\n') {
        print_newline();
        vga_update_cursor();
        return;
    } 

    if (character == '\b') {
        backspace();
        vga_update_cursor();
        return;
    }

    if (col > NUM_COLS - 1) {
        print_newline();
    }

    buffer[col + NUM_COLS * row] = (struct Char) {
        .character = character,
        .color = color,
    };
    

    col++;

    vga_update_cursor();
}

void vga_set_cursor(int x, int y) {
    row = y;
    col = x;
}

void vga_update_cursor() {
    uint16_t position = row * NUM_COLS + col;

    out(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    out(FB_DATA_PORT, (position >> 8) & 0xFF);
    out(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    out(FB_DATA_PORT, position & 0xFF);
}

void vga_enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
    out(FB_COMMAND_PORT, 0x0A);
    out(FB_DATA_PORT, cursor_start & 0x1F);

    out(FB_COMMAND_PORT, 0x0B);
    out(FB_DATA_PORT, cursor_end & 0x1F);
}

void vga_disable_cursor() {
    out(FB_COMMAND_PORT, 0x0A);
    out(FB_DATA_PORT, 0x20);
}
