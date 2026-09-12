#include "fs/ksfs.hpp"
#include <string.h>

namespace kyron::fs {
uint32_t checksum(const Superblock& block) {
    const auto* bytes = reinterpret_cast<const uint8_t*>(&block);
    uint32_t value = 2166136261u;
    for (size_t index = 0; index < sizeof(Superblock) - sizeof(uint32_t); ++index) {
        value ^= bytes[index];
        value *= 16777619u;
    }
    return value;
}

bool format(BlockDevice& device, uint32_t inode_count) {
    if (device.block_count() < 8 || inode_count == 0) return false;
    Superblock block{};
    block.magic = KSFS_MAGIC;
    block.version = KSFS_VERSION;
    block.block_size = KSFS_BLOCK_SIZE;
    block.total_blocks = static_cast<uint32_t>(device.block_count());
    block.inode_table_block = 2;
    block.inode_count = inode_count;
    block.root_inode = KSFS_ROOT_INODE;
    block.bitmap_block = 1;
    block.free_blocks = block.total_blocks - 3;
    block.checksum = checksum(block);
    uint8_t buffer[KSFS_BLOCK_SIZE]{};
    memcpy(buffer, &block, sizeof(block));
    return device.write(0, buffer);
}

bool mount(BlockDevice& device, Superblock& block) {
    uint8_t buffer[KSFS_BLOCK_SIZE]{};
    if (!device.read(0, buffer)) return false;
    memcpy(&block, buffer, sizeof(block));
    return block.magic == KSFS_MAGIC && block.version == KSFS_VERSION &&
           block.block_size == KSFS_BLOCK_SIZE && block.root_inode == KSFS_ROOT_INODE &&
           block.checksum == checksum(block);
}
}
