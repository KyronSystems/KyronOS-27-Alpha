# Windows development

KyronOS can be developed on Windows without WSL2. The repository's build commands use POSIX shell utilities (`mkdir`, `cp`, `rm`, and `command -v`), so run them from an MSYS2 terminal rather than directly from PowerShell.

## Install the Windows tools

1. Install [MSYS2](https://www.msys2.org/).
2. Open the **MSYS2 UCRT64** terminal and update its package database:

   ```sh
   pacman -Syu
   ```

   Close and reopen the terminal if MSYS2 asks you to do so, then run the update again.

3. Install the shell tools and host utilities:

   ```sh
   pacman -S --needed base-devel nasm grub xorriso qemu gdb
   ```

4. Install or add an i386 ELF freestanding toolchain. The kernel Makefile expects:

   - `gcc` and `g++` that accept `-m32` and produce ELF relocatable objects;
   - GNU `ld` that accepts `-m elf_i386`;
   - `nasm` for 32-bit assembly.

   The normal Windows MinGW linker targets Windows PE files and is not a substitute for the ELF linker used by this repository. Keep the i386 ELF toolchain's `bin` directory on `PATH` before the Windows compiler directories.

## Build from MSYS2

From the MSYS2 terminal, change to the repository directory. For a OneDrive checkout, the path usually looks like this:

```sh
cd /c/Users/<WindowsUser>/OneDrive/Documents/KyronOS
```

Then run:

```sh
./setup.sh
make kernel
make iso
make run
```

The ISO is written to `build/kyronos.iso`. To debug it, use:

```sh
make debug
```

In another MSYS2 terminal, connect GDB to QEMU:

```sh
gdb build/kyronos.kernel
(gdb) target remote :1234
(gdb) continue
```

## Host tests

The KSFS tests are ordinary C++ host tests and can be built from the same MSYS2 terminal:

```sh
make test
```

## PowerShell note

PowerShell is fine for editing the source and running Git commands. Use the MSYS2 terminal for `setup.sh`, `make`, ISO creation, and the POSIX build commands in the Makefile.
