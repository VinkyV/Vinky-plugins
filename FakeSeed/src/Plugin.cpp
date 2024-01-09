#include <ll/api/plugin/NativePlugin.h>

#include "Plugin.h"

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#include <ll/api/plugin/Plugin.h>
#include <ll/api/memory/Hook.h>
#include <ll/api/memory/Memory.h>
#include <mc/network/packet/StartGamePacket.h>

#define CONF_PATH "./plugins/FakeSeed/seed.txt"
int seed = -777;

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {
    mSelf.getLogger().info("Loaded!");

    try
    {
        if (!std::filesystem::exists(CONF_PATH))
        {
            std::filesystem::create_directories("./plugins/FakeSeed");
            std::ofstream fout(CONF_PATH);
            fout << seed << std::endl;
        }
        else
        {
            std::ifstream fin(CONF_PATH);
            if (fin)
                fin >> seed;
        }
    }
    catch(...)  { }
}

bool Plugin::enable() {
    mSelf.getLogger().info("Seed is set to " + std::to_string(seed));
    return true;
}

bool Plugin::disable() {
    return true;
}

} // namespace plugin

// Hook Writing StartGamePacket
LL_AUTO_STATIC_HOOK(
    FakeSeedHook,
    ll::memory::HookPriority::Normal,
    "?write@StartGamePacket@@UEBAXAEAVBinaryStream@@@Z",
    void*,
    StartGamePacket* pkt,
    BinaryStream* bs
) {
    pkt->mSettings.mSeed.mValue = uint64_t(seed);
    return origin(pkt, bs);
}