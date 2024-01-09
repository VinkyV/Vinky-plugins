#include <ll/api/plugin/NativePlugin.h>

#include "Plugin.h"

#include "Config.h"

#include <fmt/format.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <ll/api/Config.h>
#include <ll/api/memory/Hook.h>
#include <ll/api/memory/Memory.h>
#include <ll/api/plugin/Plugin.h>
#include <ll/api/service/Bedrock.h>
#include <mc/server/commands/MinecraftCommands.h>
#include <mc/server/commands/ServerCommandOrigin.h>
#include <mc/world/Minecraft.h>
#include <mc/world/level/Level.h>

#include <mc/server/ServerPlayer.h>
#include <mc/network/NetworkIdentifier.h>
#include <mc/network/ServerNetworkHandler.h>
#include <mc/network/packet/TextPacket.h>
#include <mc/world/level/block/Block.h>
#include <mc/world/level/block/actor/BlockActor.h>
#include <mc/world/item/registry/ItemStack.h>
#include <mc/world/level/dimension/Dimension.h>
#include <mc/world/level/block/actor/DropperBlockActor.h>
#include <mc/world/level/BlockSource.h>
#include <mc/math/vector/intN.h>
#include <mc/world/Container.h>
#include <mc/world/item/registry/ItemStack.h>
#include <mc/world/item/Item.h>
#include <mc/world/item/ItemStackBase.h>
#include <mc/world/containers/models/LevelContainerModel.h>

#include "Database.h"

using std::cout;
using std::endl;
using std::string;


Config mConfig;

MCRESULT runCmd(const string& cmd) {
  std::unique_ptr<CommandOrigin> origin = std::make_unique<ServerCommandOrigin>(
    ServerCommandOrigin("Server", ll::service::getLevel()->asServer(), CommandPermissionLevel::Internal, 0)
  );
  CommandContext context = CommandContext(cmd, std::move(origin), CommandVersion::CurrentVersion);
  return ll::service::getMinecraft()->getCommands().executeCommand(context, false);
}

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {
  const auto& configFilePath = self.getConfigDir() / "config.json";
  if (!ll::config::loadConfig(mConfig, configFilePath)) {
    mSelf.getLogger().warn("Cannot load configurations from {}", configFilePath);
    mSelf.getLogger().info("Saving default configurations");

    if (!ll::config::saveConfig(mConfig, configFilePath)) {
      mSelf.getLogger().error("Cannot save default configurations to {}", configFilePath);
    }
  }

  initDB();

  mSelf.getLogger().info("Loaded!");
}

bool Plugin::enable() {
  return true;
}

bool Plugin::disable() {
  return true;
}

} // namespace plugin

Dimension* getDimensionPtr(AutomaticID<Dimension, int> dimId) {
  return ll::service::getLevel()->getDimension(dimId).get();
}

BlockSource* getBlockSource(int dimID) {
  auto dim = getDimensionPtr(dimID);
  return &dim->getBlockSourceFromMainChunkSource();
}

Container* getContainer(Vec3 pos, int dim) {
  return ((DropperBlockActor*)nullptr)->getContainerAt(*getBlockSource(dim), pos);
}

LL_AUTO_TYPED_INSTANCE_HOOK(
    PlayerDestroyBlockHook,
    HookPriority::Normal,
    Block,
    &Block::playerWillDestroy,
    Block const*,
    Player&         player,
    BlockPos const& pos
) {
  int x = pos.get<int>(0), y = pos.get<int>(1), z = pos.get<int>(2), dim = player.getDimensionId();
  if (this->getTypeName() == "minecraft:chest" || this->getTypeName() == "minecraft:trapped_chest") {
    Container* ct = getContainer(pos.center(), player.getDimensionId());
    std::vector<ItemStack const*> items = ct->getSlots();
    for (int i = 0;i < items.size();i++) {
      if (!items[i]->isNull()) {
        cout << items[i]->getTypeName() << " " << x << " " << y << " " << z << " " << dim << endl;
      }
    }
  }
  cout << this->getTypeName() << " " << x << " " << y << " " << z << " " << dim << endl;
  cout << "Player break block" << endl;
  return origin(player, pos);
}

LL_AUTO_TYPED_INSTANCE_HOOK(
  ContainerChangedHook,
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