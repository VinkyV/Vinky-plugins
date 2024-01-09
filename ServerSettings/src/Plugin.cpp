#include <ll/api/plugin/NativePlugin.h>

#include "Plugin.h"

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#include <ll/api/plugin/Plugin.h>
#include <ll/api/memory/Hook.h>
#include <ll/api/memory/Memory.h>

#include <mc/network/ServerNetworkHandler.h>
#include <mc/network/NetworkIdentifier.h>
#include <mc/network/packet/ServerSettingsResponsePacket.h>
#include <mc/deps/core/utility/BinaryStream.h>
#include <mc/network/MinecraftPackets.h>
#include <mc/server/ServerPlayer.h>
#include <ll/api/schedule/Scheduler.h>

std::string data = "";;

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {
    mSelf.getLogger().info("Loaded!");

    std::ifstream fs;
    fs.open("plugins/ServerSettings/config.json", std::ios::in);
    if (fs) {
        char buf[1024];
        while (fs.getline(buf, 1024)) {
            data.append(buf);
        }
    }
    else {
        mSelf.getLogger().error("[ServerSettings] Failed to read config.json!");
    }
}

bool Plugin::enable() {
    return true;
}

bool Plugin::disable() {
    return true;
}

} // namespace plugin

using namespace ll::chrono_literals;

ll::schedule::GameTimeAsyncScheduler scheduler;

// Hook Writing StartGamePacket
LL_AUTO_STATIC_HOOK(
    ServerSettingsRequestHook,
    ll::memory::HookPriority::Normal,
    "?handle@?$PacketHandlerDispatcherInstance@VServerSettingsRequestPacket@@$0A@@@UEBAXAEBVNetworkIdentifier@@AEAVNetEventCallback@@AEAV?$shared_ptr@VPacket@@@std@@@Z",
    void,
    NetworkIdentifier* ni,
    ServerNetworkHandler* snh,
    void* pkt
) {
    scheduler.add<ll::schedule::DelayTask>(2s, [&] {
      ServerSettingsResponsePacket packet = ServerSettingsResponsePacket{};
      packet.mFormId = 0;
      packet.mFormJSON.assign(data);
      packet.sendToClients();
    });
    return origin(ni, snh, pkt);
}