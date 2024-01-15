#include "Plugin.h"

#include <iostream>
#include <string>

#include <ll/api/plugin/Plugin.h>
#include <ll/api/memory/Hook.h>
#include <ll/api/memory/Memory.h>

#include <mc/codebuilder/MCRESULT.h>
#include <ll/api/service/Bedrock.h>
#include <ll/api/command/DynamicCommand.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <mc/server/commands/MinecraftCommands.h>
#include <mc/server/commands/ServerCommandOrigin.h>
#include <mc/server/commands/CommandContext.h>
#include <mc/world/Minecraft.h>
#include <mc/world/level/Level.h>
#include <ll/api/event/world/ServerStartedEvent.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/player/PlayerJoinEvent.h>
#include <ll/api/event/player/PlayerLeaveEvent.h>
#include <mc/world/actor/player/Player.h>

using namespace ll::event;

MCRESULT runCmd(const std::string& cmd) {
  std::unique_ptr<CommandOrigin> origin = std::make_unique<ServerCommandOrigin>(
    ServerCommandOrigin("Server", ll::service::getLevel()->asServer(), CommandPermissionLevel::Internal, 0)
  );
  CommandContext context = CommandContext(cmd, std::move(origin), CommandVersion::CurrentVersion);
  return ll::service::getMinecraft()->getCommands().executeCommand(context, false);
}

class TestCommand : public Command {
public:
    void execute(CommandOrigin const& /*ori*/, CommandOutput& output) const override {
      output.success("123");
    }

    static void setup(CommandRegistry& registry) {
        registry.registerCommand("testcommand", "LeviLamina CommandRegistry Test", CommandPermissionLevel::Any);
        registry.registerOverload<TestCommand>("testcommand");
    }
};
namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {
  auto& bus = EventBus::getInstance();

  mPlayerJoinEventListener = bus.emplaceListener<PlayerJoinEvent>([](PlayerJoinEvent& ev) {
    ev.self().sendMessage("Welcome");
  });

  mPlayerLeaveEventListener = bus.emplaceListener<PlayerLeaveEvent>([](PlayerLeaveEvent& ev) {
    std::cout << "Left!" << std::endl;
  });

  mServerStartedEventListener = bus.emplaceListener<ServerStartedEvent>([](ServerStartedEvent& ev) {
    auto registry = ll::service::getCommandRegistry();
    TestCommand::setup(registry);
    std::cout << "Server started!" << std::endl;
  });
}

bool Plugin::enable() {
  mSelf.getLogger().info("Loaded!");

  return true;
}

bool Plugin::disable() {
  auto& bus = EventBus::getInstance();
  bus.removeListener(mPlayerJoinEventListener);
  bus.removeListener(mPlayerLeaveEventListener);
  bus.removeListener(mServerStartedEventListener);
  return true;
}

} // namespace plugin