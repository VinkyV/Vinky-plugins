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
LL_AUTO_TYPED_INSTANCE_HOOK(
  ContainerChangedBookHook,
  HookPriority::Normal,
  LevelContainerModel,
  "?_onItemChanged@LevelContainerModel@@MEAAXHAEBVItemStack@@0@Z",
  void,
  int slotNumber,
  ItemStack* oldItem,
  ItemStack* newItem
) {
  auto* player = (Player*)ll::memory::dAccess<Actor*>(this, 208);
  if (player->hasOpenContainer()) {
    int slot = slotNumber + this->_getContainerOffset();
    BlockPos* pos = (BlockPos*)((char*)this + 216);
    int x = pos->get<int>(0), y = pos->get<int>(1), z = pos->get<int>(2), dim = player->getDimensionId();
    cout << slot << " " << x << " " << y << " " << z << " " << dim << " " << oldItem->getTypeName() << " " << (int)oldItem->mCount << " " << newItem->getTypeName() << " " << (int)newItem->mCount << endl;
  }
}