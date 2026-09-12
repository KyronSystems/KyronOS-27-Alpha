# Boot

The ISO uses GRUB's Multiboot2 protocol. `boot/entry.asm` validates the entry ABI enough to pass the Multiboot magic and information pointer to `kmain`. The linker keeps the Multiboot header at the beginning of the loaded image. The next boot milestone is setting up page tables and a GDT before entering a true x86-64 kernel entry point.
