#include "Plugin.h"

#include <iostream>
#include <string>
#include <ctime>
#include <thread>
#include <chrono>

#include <ll/api/Config.h>
#include <ll/api/plugin/Plugin.h>
#include <ll/api/service/Bedrock.h>
#include <mc/codebuilder/MCRESULT.h>
#include <mc/server/commands/ServerCommandOrigin.h>
#include <mc/server/commands/MinecraftCommands.h>
#include <mc/world/Minecraft.h>
#include <mc/server/commands/CommandContext.h>
#include <mc/world/level/Level.h>
#include <mc/network/packet/TextPacket.h>
#include <mc/world/actor/player/Player.h>
#include <mc/network/NetworkIdentifier.h>
#include <mc/network/ServerNetworkHandler.h>

bool isWorking = false;

MCRESULT runCmd(const std::string& cmd) {
  std::unique_ptr<CommandOrigin> origin = std::make_unique<ServerCommandOrigin>(
    ServerCommandOrigin("Server", ll::service::getLevel()->asServer(), CommandPermissionLevel::Internal, 0)
  );
  CommandContext context = CommandContext(cmd, std::move(origin), CommandVersion::CurrentVersion);
  return ll::service::getMinecraft()->getCommands().executeCommand(context, false);
}

bool kick(Player* pl, const std::string& msg) {
    NetworkIdentifier const& pNetworkIdentifier = pl->getNetworkIdentifier();
    ll::service::getServerNetworkHandler()->disconnectClient(pNetworkIdentifier, Connection::DisconnectFailReason(0), msg, 0);
    return true;
}

std::vector<Player*> getAllPlayers() {
    try {
        std::vector<Player*> player_list;
        ll::service::getLevel()->forEachPlayer([&](Player& sp) -> bool {
            Player* player = &sp;
            player_list.push_back(player);
            return true;
        });
        return player_list;
    } catch (...) { return {}; }
}

void broadcast(std::string message) {
    TextPacket::createRawMessage(message).sendToClients();
}

void task(int wait) {
    auto level = ll::service::getLevel();
    std::this_thread::sleep_for(std::chrono::milliseconds(wait * 60 * 1000));

    broadcast("§d| §bРестарт сервера через 10 секунд!");
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    std::vector<Player*> players = getAllPlayers();
    for (Player* p : players) {
        kick(p, "§bРестарт сервера!");
    }
    ll::service::getMinecraft()->requestServerShutdown("");
}

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {
  const auto& configFilePath = "./plugins/AutoRestart/config/config.json";
  if (!ll::config::loadConfig(mConfig, configFilePath)) {
    mSelf.getLogger().warn("Cannot load configurations from {}", configFilePath);
    mSelf.getLogger().info("Saving default configurations");

    if (!ll::config::saveConfig(mConfig, configFilePath)) {
      mSelf.getLogger().error("Cannot save default configurations to {}", configFilePath);
    }
  }
}

bool Plugin::enable() {
    mSelf.getLogger().info("Loaded!");
    return true;
}

bool Plugin::disable() {
    return true;
}

} // namespace plugin

#include <mc/world/level/storage/DBStorage.h>
#include <mc/world/level/storage/SnapshotFilenameAndLength.h>

LL_AUTO_STATIC_HOOK(
    SnapshotCreateHook,
    ll::memory::HookPriority::Normal,
    "?createSnapshot@DBStorage@@UEAA?AV?$vector@USnapshotFilenameAndLength@@V?$allocator@USnapshotFilenameAndLength@@@std@@@std@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@3@@Z",
    vector<SnapshotFilenameAndLength>&,
    DBStorage* _this,
    vector<SnapshotFilenameAndLength>& fileData,
    string& worldName
){
    if (isWorking) {
        auto& files = origin(_this, fileData, worldName);
        return files;
    }
    else {
        isWorking = true;
        return origin(_this, fileData, worldName);
    }
}

LL_AUTO_STATIC_HOOK(
    SnapshotReleaseHook,
    ll::memory::HookPriority::Normal,
    "?releaseSnapshot@DBStorage@@UEAAXXZ",
    void,
    DBStorage* _this
){
    isWorking = false;
    return origin(_this);
}