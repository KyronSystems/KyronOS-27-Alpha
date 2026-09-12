#pragma once
#include <stdint.h>

namespace console {
void clear();
void write(const char* text, uint8_t color = 0x07);
void write_line(const char* text, uint8_t color = 0x07);
void write_char(char character, uint8_t color = 0x07);
void backspace();
}
