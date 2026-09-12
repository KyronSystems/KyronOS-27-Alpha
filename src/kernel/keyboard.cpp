#include "kernel/keyboard.hpp"
#include "kernel/console.hpp"

namespace {
bool shift_down = false;

uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

char translate(uint8_t scan) {
    if (scan == 0x0E) return '\b';
    if (scan == 0x1C) return '\n';
    if (scan == 0x0F) return '\t';
    if (scan == 0x39) return ' ';
    if (scan >= 0x02 && scan <= 0x0D) return shift_down ? "!@#$%^&*()_+"[scan - 0x02] : "1234567890-="[scan - 0x02];
    if (scan >= 0x10 && scan <= 0x19) { char key = "qwertyuiop"[scan - 0x10]; return shift_down ? key - 'a' + 'A' : key; }
    if (scan == 0x1A) return shift_down ? '{' : '[';
    if (scan == 0x1B) return shift_down ? '}' : ']';
    if (scan >= 0x1E && scan <= 0x26) { char key = "asdfghjkl"[scan - 0x1E]; return shift_down ? key - 'a' + 'A' : key; }
    if (scan == 0x27) return shift_down ? ':' : ';';
    if (scan == 0x28) return shift_down ? '"' : '\'';
    if (scan == 0x29) return shift_down ? '~' : '`';
    if (scan == 0x2B) return shift_down ? '|' : '\\';
    if (scan >= 0x2C && scan <= 0x35) return shift_down ? "ZXCVBNM<>?"[scan - 0x2C] : "zxcvbnm,./"[scan - 0x2C];
    return 0;
}
}

namespace keyboard {
char read_key() {
    while ((inb(0x64) & 1) == 0) {}
    uint8_t scan = inb(0x60);
    if (scan == 0x2A || scan == 0x36) { shift_down = true; return 0; }
    if (scan == 0xAA || scan == 0xB6) { shift_down = false; return 0; }
    if (scan & 0x80) return 0;
    return translate(scan);
}

void read_line(char* buffer, uint32_t capacity, Completion completion) {
    uint32_t length = 0;
    while (length + 1 < capacity) {
        char key = read_key();
        if (key == 0) continue;
        if (key == '\n') {
            buffer[length] = '\0';
            console::write_line("");
            return;
        }
        if (key == '\b') {
            if (length > 0) { --length; console::backspace(); }
            continue;
        }
        if (key == '\t') {
            if (completion) {
                uint32_t completed = completion(buffer, length, capacity);
                while (length < completed) console::write_char(buffer[length++]);
            }
            continue;
        }
        buffer[length++] = key;
        console::write_char(key);
    }
    buffer[length] = '\0';
    console::write_line("");
}
}
