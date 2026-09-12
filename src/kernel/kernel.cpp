#include "kernel/console.hpp"
#include "kernel/shell.hpp"
#include <stdint.h>

extern "C" void kmain(uint32_t magic, uint32_t multiboot_info) {
    (void)multiboot_info;
    console::clear();
    console::write_line("+----------------------------------------------------------+", 0x09);
    console::write_line("|                 KYRONOS 27 ALPHA 1                      |", 0x0D);
    console::write_line("|                 KSFS SYSTEM                             |", 0x0B);
    console::write_line("+----------------------------------------------------------+", 0x09);
    console::write_line("");
    if (magic != 0x36D76289) {
        console::write_line("Boot error: invalid Multiboot2 magic.", 0x0C);
        return;
    }
    console::write_line("Initializing CPU ........ OK", 0x0B);
    console::write_line("Initializing console ... OK", 0x0B);
    console::write_line("USB: controller probing deferred", 0x0D);
    console::write_line("CD/DVD: provided by boot media", 0x0D);
    console::write_line("Mounting KSFS ........... pending storage driver", 0x0D);
    console::write_line("");
    shell::run();
}
