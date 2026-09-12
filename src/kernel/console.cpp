#include "kernel/console.hpp"

namespace {
volatile uint16_t* const vga = reinterpret_cast<volatile uint16_t*>(0xB8000);
uint8_t row = 0;
uint8_t column = 0;

void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

void update_cursor() {
    uint16_t position = static_cast<uint16_t>(row * 80 + column);
    outb(0x3D4, 0x0F);
    outb(0x3D5, static_cast<uint8_t>(position));
    outb(0x3D4, 0x0E);
    outb(0x3D5, static_cast<uint8_t>(position >> 8));
}

void scroll() {
    for (uint8_t screen_row = 1; screen_row < 25; ++screen_row)
        for (uint8_t screen_column = 0; screen_column < 80; ++screen_column)
            vga[(screen_row - 1) * 80 + screen_column] = vga[screen_row * 80 + screen_column];
    for (uint8_t screen_column = 0; screen_column < 80; ++screen_column)
        vga[24 * 80 + screen_column] = 0x0700 | ' ';
    row = 24;
    column = 0;
}

void put(char character, uint8_t color) {
    if (character == '\n') { ++row; column = 0; if (row >= 25) scroll(); update_cursor(); return; }
    if (column >= 80) { ++row; column = 0; if (row >= 25) scroll(); }
    vga[row * 80 + column++] = static_cast<uint16_t>(character) | (static_cast<uint16_t>(color) << 8);
    update_cursor();
}
}

namespace console {
void clear() {
    for (uint16_t index = 0; index < 80 * 25; ++index) vga[index] = 0x0700 | ' ';
    row = 0; column = 0;
    update_cursor();
}
void write(const char* text, uint8_t color) { while (*text) put(*text++, color); }
void write_line(const char* text, uint8_t color) { write(text, color); put('\n', color); }
void write_char(char character, uint8_t color) { put(character, color); }
void backspace() {
    if (column == 0) return;
    --column;
    vga[row * 80 + column] = 0x0700 | ' ';
    update_cursor();
}
}
