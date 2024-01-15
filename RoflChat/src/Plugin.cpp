#include <ll/api/plugin/NativePlugin.h>

#include "Plugin.h"

#include <iostream>
#include <string>

#include <ll/api/Config.h>
#include "ll/api/memory/Hook.h"
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

#include <WebSocketClient.h>

using namespace cyanray;

using std::cout;
using std::endl;
using std::string;

WebSocketClient client;

MCRESULT runCmd(const string& cmd) {
  std::unique_ptr<CommandOrigin> origin = std::make_unique<ServerCommandOrigin>(
    ServerCommandOrigin("Server", ll::service::getLevel()->asServer(), CommandPermissionLevel::Internal, 0)
  );
  CommandContext context = CommandContext(cmd, std::move(origin), CommandVersion::CurrentVersion);
  return ll::service::getMinecraft()->getCommands().executeCommand(context, false);
}

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {
  mSelf.getLogger().info("Loaded!");
}

bool Plugin::enable() {
  string ws_uri = "ws://127.0.0.1:1422";
  try {
    client.Connect(ws_uri);
    mSelf.getLogger().info("Connected!");
  } catch (const std::exception& ex) {
    mSelf.getLogger().error(ex.what());
  }

  client.OnTextReceived([](WebSocketClient& client, string text) { runCmd(text); });

  auto* mSelfs = &mSelf;

  client.OnLostConnection([ws_uri, mSelfs](WebSocketClient& client, int code) {
    mSelfs->getLogger().info("Lost connection!");
    while (true)
    {
      try
      {
        client.Connect(ws_uri);
        mSelfs->getLogger().info("Reconnected.");
        break;
      }
      catch (const std::exception& ex) {}
    }
  });

  return true;
}

bool Plugin::disable() {
  client.Close();
  return true;
}

} // namespace plugin

LL_AUTO_TYPED_INSTANCE_HOOK(
  SendMessageHook,
  ll::memory::HookPriority::Normal,
  ServerNetworkHandler,
  "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVTextPacket@@@Z",
  void,
  NetworkIdentifier const& identifier,
  TextPacket const& packet
) {
  if (ServerPlayer* player = getServerPlayer(identifier, packet.mClientSubId); player) {
    if (client.GetStatus() == WebSocketClient::Status::Open) {
      client.SendText(const_cast<TextPacket&>(packet).mMessage);
    }
  }
  origin(identifier, packet);
}