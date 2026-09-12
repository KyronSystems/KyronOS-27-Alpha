#include "fs/ksfs.hpp"
#include <array>
#include <cassert>
#include <cstring>

class MemoryDevice final : public kyron::fs::BlockDevice {
    std::array<std::array<uint8_t, kyron::fs::KSFS_BLOCK_SIZE>, 16> blocks{};
public:
    bool read(uint64_t block, void* buffer) override {
        if (block >= blocks.size()) return false;
        std::memcpy(buffer, blocks[block].data(), kyron::fs::KSFS_BLOCK_SIZE); return true;
    }
    bool write(uint64_t block, const void* buffer) override {
        if (block >= blocks.size()) return false;
        std::memcpy(blocks[block].data(), buffer, kyron::fs::KSFS_BLOCK_SIZE); return true;
    }
    uint64_t block_count() const override { return blocks.size(); }
};

int main() {
    MemoryDevice device;
    assert(kyron::fs::format(device));
    kyron::fs::Superblock superblock{};
    assert(kyron::fs::mount(device, superblock));
    assert(superblock.magic == kyron::fs::KSFS_MAGIC);
    assert(superblock.free_blocks == 13);
    return 0;
}
