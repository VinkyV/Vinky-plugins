#include <llapi/HookAPI.h>
#include <llapi/Global.h>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/BinaryStream.hpp>
#include <llapi/mc/BlockPalette.hpp>
#include <llapi/mc/Block.hpp>
#include <llapi/mc/CompoundTag.hpp>
#include <llapi/mc/ItemRegistry.hpp>

void PluginInit() {
    std::cout << "Loaded" << std::endl;
    auto palette = &Global<Level>->getBlockPalette();
    auto stream = new BinaryStream();
    for (unsigned int i = 0; i < sizeof(palette); i++) {
        CompoundTag state = palette->getBlock(i).getData();
        stream->writeType(state->);
    }
    std::ofstream output("mapping_files/block_states.nbt");
    output << stream->buffer;
    output.close();
    delete stream;
}