# Roadmap

## KyronOS

- [x] Project structure and build pipeline
- [x] Multiboot2 kernel bootstrap
- [x] Text console identity
- [x] KSFS superblock format and validation core
- [x] Generic block-device interface
- [ ] x86-64 long-mode entry and memory map parsing
- [ ] PS/2 keyboard and interactive shell
- [ ] KSFS inode, directory, and file operations
- [ ] `ksfs-mkfs` image tool
- [ ] USB EHCI/xHCI detection
- [ ] CD/DVD read support
- [ ] Full command set and `notepad`
- [ ] Hard-drive drivers
- [ ] GUI or desktop environment

Later releases can add networking, richer userland, storage drivers, and eventually a graphical subsystem. Unsupported behavior must remain reported as unsupported until implemented.
