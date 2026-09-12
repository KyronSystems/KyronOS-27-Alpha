# KSFS format

KSFS is an original, little-endian, fixed-block format. Every block is 4096 bytes. Block 0 is the superblock, block 1 is the free-block bitmap, and blocks beginning at block 2 hold the inode table. Directory and file data follow the inode table.

The superblock contains: magic `0x4B534653` (`KSFS`), version `1`, block size, total blocks, free blocks, inode-table block, inode count, root inode `1`, bitmap block, and an FNV-1a checksum over all fields except checksum. `BlockDevice` supplies full-block reads and writes, keeping KSFS independent of controller hardware.

The current implementation formats and validates a superblock. Inode allocation, directory entries, file extents, deletion, and `ksfs-mkfs` population are the next filesystem milestone; no command claims those features yet.
