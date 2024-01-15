#include <ll/api/plugin/NativePlugin.h>

#include "Plugin.h"

#include <fmt/format.h>

#include <ostream>
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
#include <mc/world/level/BlockEventCoordinator.h>
#include <mc/nbt/CompoundTag.h>
#include <mc/nbt/Tag.h>
#include <mc/nbt/ListTag.h>

//#include "Database.h"

#define datapath "./logs/PagodaProtect/"

MCRESULT runCmd(const std::string& cmd) {
  std::unique_ptr<CommandOrigin> origin = std::make_unique<ServerCommandOrigin>(
    ServerCommandOrigin("Server", ll::service::getLevel()->asServer(), CommandPermissionLevel::Internal, 0)
  );
  CommandContext context = CommandContext(cmd, std::move(origin), CommandVersion::CurrentVersion);
  return ll::service::getMinecraft()->getCommands().executeCommand(context, false);
}

std::string GetFullDateTime()
{
    time_t t = time(NULL);
    tm ts;
    localtime_s(&ts, &t);
    char buf[24] = { 0 };
    strftime(buf, 24, "%Y-%m-%d %H:%M:%S", &ts);
    return std::string(buf);
}

std::string GetDateTime()
{
    time_t t = time(NULL);
    tm ts;
    localtime_s(&ts, &t);
    char buf[24] = { 0 };
    strftime(buf, 24, "%Y-%m-%d", &ts);
    return std::string(buf);
}

void logToFile(std::string message) {
    std::ofstream logFile(datapath + GetDateTime() + "-PagodaProtect.log", std::ios::app);
    if (logFile.is_open()) {
      logFile << "[" << GetFullDateTime() << "] " << message << std::endl;
      logFile.close();
    } else {
      std::cerr << "Error: Unable to open log file." << std::endl;
    }
}

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {

  //initDB();

  mSelf.getLogger().info("By: VinkyV#7660");
}

bool Plugin::enable() {
  mSelf.getLogger().info("Succesfully loaded!");
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

LL_AUTO_TYPE_INSTANCE_HOOK(
  PlayerPlaceBlockHook,
  HookPriority::Normal,
  BlockEventCoordinator,
  &BlockEventCoordinator::sendBlockPlacedByPlayer,
  void,
  Player& player,
  Block const& blk,
  BlockPos const& pos,
  bool underwater
){
  int x = pos.get<int>(0), y = pos.get<int>(1), z = pos.get<int>(2), dim = player.getDimensionId();
  logToFile(fmt::format("[BlockPlaced] player: {} block: {}, x: {}, y: {}, z: {}, dim: {}", player.getRealName(), blk.getTypeName(), x, y, z, dim));
}

LL_AUTO_TYPE_INSTANCE_HOOK(
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
    std::vector<ItemStack const*> itemList = ct->getSlots();
    for (int i = 0;i < itemList.size();i++) {
      if (!itemList[i]->isNull()) {
        //logBlock(player, itemList[i]->getId(), dim, x, y, z, 3, (int)itemList[i]->mAuxValue, (int)itemList[i]->mCount, i, true);
        //std::cout << itemList[i]->getTypeName() << " " << x << " " << y << " " << z << " " << dim << std::endl;
        logToFile(fmt::format("[ItemDropAfterDestroy] player: {} item: {}, x: {}, y: {}, z: {}, dim: {}, count: {}x, slot: {} nbt: {}", player.getRealName(), itemList[i]->getTypeName(), x, y, z, dim, (int)itemList[i]->mCount, i, itemList[i]->save()->toSnbt(SnbtFormat::Minimize)));
      }
    }
  }
  //logBlock(player, -1, dim, x, y, z, 1, -1, -1, -1, false);
  logToFile(fmt::format("[BlockDestroyed] player: {} block: {}, x: {}, y: {}, z: {}, dim: {}", player.getRealName(), this->getTypeName(), x, y, z, dim));
  return origin(player, pos);
}

LL_AUTO_TYPE_INSTANCE_HOOK(
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
    logToFile(fmt::format("[ItemChanged] player: {} slot:{}, x: {}, y: {}, z: {}, dim: {}, oldItem: {} {}x nbt: {}, newItem: {} {}x nbt: {}", player->getRealName(), slot, x, y, z, dim, oldItem->getTypeName(), (int)oldItem->mCount, oldItem->save()->toSnbt(SnbtFormat::Minimize), newItem->getTypeName(), (int)newItem->mCount, newItem->save()->toSnbt(SnbtFormat::Minimize)));
  }
}

