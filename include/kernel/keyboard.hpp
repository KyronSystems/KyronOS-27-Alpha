#pragma once
#include <stdint.h>

namespace keyboard {
char read_key();
using Completion = uint32_t (*)(char* buffer, uint32_t length, uint32_t capacity);
void read_line(char* buffer, uint32_t capacity, Completion completion = nullptr);
}
