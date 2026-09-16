# KyronOS

KyronOS is a small, text-only experimental operating system for x86 PCs. It boots from a GRUB Multiboot2 ISO and provides a VGA terminal with a shell, keyboard input, directory navigation, users, and a RAM-backed filesystem prototype.

## Real hardware

Video of KyronOS running on real hardware:

[Watch `real_hardware_2000.mp4`](.github/assets/real_hardware_2000.mp4)

<video src=".github/assets/real_hardware_2000.mp4" controls></video>

## Build

Verify the host tools without installing anything:

```sh
./setup.sh
```

On Fedora:

```sh
sudo dnf install gcc gcc-c++ make nasm grub2-tools-extra xorriso qemu-system-x86-core qemu-ui-gtk gdb
```

On Debian or Ubuntu:

```sh
sudo apt install build-essential nasm grub-pc-bin grub-common xorriso qemu-system-x86 gdb
```

Build and run the ISO:

```sh
make iso
make run
```

The generated image is `build/kyronos.iso`. For debugging, run `make debug`, then connect with:

```sh
gdb build/kyronos.kernel
(gdb) target remote :1234
```

## Shell

The current shell includes:

```text
help clear version about echo
ls cd pwd go back go home go root
touch mkdir rmdir write append cat rm
add user <name>
switch user <name>
devices mem reboot shutdown
```

The default root layout is:

```text
/
├── kyron/
├── boot/
├── system/
├── etc/
├── home/
├── tmp/
├── dev/
└── bin/
```

User home directories live under `/home`. Inside the active user's home, the prompt uses `~`, for example `[kyron]:~#` and `[kyron]:~/projects#`.

## Project status

Implemented foundations include the Multiboot2 boot image, VGA console with scrolling and cursor tracking, PS/2 keyboard input with Shift and Tab completion, shell navigation, user home-directory handling, KSFS superblock validation, and a generic block-device interface.

KyronOS does not yet provide native EHCI/xHCI controller transport, USB HID device enumeration, persistent disk drivers, a complete on-disk KSFS file tree, or a graphical environment. USB boot-protocol decoding is present, but native USB keyboard support requires the controller and interrupt-transfer layers.

See [docs/BUILDING.md](docs/BUILDING.md), [docs/WINDOWS.md](docs/WINDOWS.md), [docs/KSFS.md](docs/KSFS.md), [docs/USB.md](docs/USB.md), and [docs/ROADMAP.md](docs/ROADMAP.md) for details.
