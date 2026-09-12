# Building

1. Run `./setup.sh`.
2. Install the printed prerequisites if checks fail. On Fedora, use `sudo dnf install gcc gcc-c++ make nasm grub2-tools-extra xorriso qemu-system-x86-core qemu-ui-gtk gdb`. On Debian/Ubuntu, use `sudo apt install build-essential nasm grub-pc-bin grub-common xorriso qemu-system-x86 gdb`.
3. Run `make kernel`.
4. Run `make iso` to create `build/kyronos-alpha1.iso`.
5. Run `make run` to boot it in QEMU.
6. Run `make debug`, then connect GDB to `localhost:1234`.

The build uses GRUB Multiboot2 and NASM. Fedora names the rescue command `grub2-mkrescue`; the Makefile detects both Fedora and Debian/Ubuntu names. A host compiler may produce the bootstrap ELF, but a freestanding cross compiler should be used for future kernel expansion. The setup script never installs packages automatically.
