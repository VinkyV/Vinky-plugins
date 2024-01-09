#include <ll/api/plugin/NativePlugin.h>

#include "Plugin.h"

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#include <ll/api/memory/Hook.h>
#include <ll/api/memory/Memory.h>
#include <ll/api/service/Bedrock.h>
#include <mc/entity/utilities/ActorType.h>
#include <ll/api/command/DynamicCommand.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <mc/world/level/Level.h>
#include <utility>
#include <sys/timeb.h>

int tick = 0;
const float resetTicks = 20.0;
long timeOld = 0;
long timeNew = 0;

long getTimestamp() {
	timeb t;
	ftime(&t);
	return (long)t.time * 1000 + t.millitm;
}

std::string getTps() {
	double input = 1000.0 * (resetTicks / (timeNew - timeOld));
	auto output = std::to_string(input);
	if (input >= 18.5) {
		return "[§a#§r] TPS: §a" + output;
	}
	else if (input > 15.0) {
		return "[§a#§r] TPS: §6" + output;
	}
	else return "[§a#§r] TPS: §c" + output;
}

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {
    mSelf.getLogger().info("Loaded!");
}

bool Plugin::enable() {
    auto& logger = mSelf.getLogger();

    auto commandRegistry = ll::service::getCommandRegistry();
    if (!commandRegistry) {
        throw std::runtime_error("failed to get command registry");
    }

    auto tpsCommand =
        DynamicCommand::createCommand(commandRegistry, "tps", "Server tps", CommandPermissionLevel::Any);
    tpsCommand->addOverload();
    tpsCommand->setCallback(
        [&logger](DynamicCommand const&, CommandOrigin const& origin, CommandOutput& output, std::unordered_map<std::string, DynamicCommand::Result>&) {
            output.success(getTps());
        }
    );
    DynamicCommand::setup(commandRegistry, std::move(tpsCommand));

    auto maidCommand =
        DynamicCommand::createCommand(commandRegistry, "maid", "§dServer maid", CommandPermissionLevel::GameDirectors);
    maidCommand->addOverload();
    maidCommand->setCallback(
        [&logger](DynamicCommand const&, CommandOrigin const& origin, CommandOutput& output, std::unordered_map<std::string, DynamicCommand::Result>&) {
            auto level = ll::service::getLevel();
            std::string message = "";
            message += getTps();
            message += "\n§r[§a#§r] Players: §b" + std::to_string(level->getActivePlayerCount());
            message += "\n§r[§a#§r] Entities: §b" + std::to_string(level->getEntities().size());
            output.success(message);
        }
    );
    DynamicCommand::setup(commandRegistry, std::move(maidCommand));

    return true;
}

bool Plugin::disable() {
    auto commandRegistry = ll::service::getCommandRegistry();
    if (!commandRegistry) {
        throw std::runtime_error("failed to get command registry");
    }

    commandRegistry->unregisterCommand("tps");
    commandRegistry->unregisterCommand("maid");
    return true;
}

} // namespace plugin

// Hook Level tick func
LL_AUTO_STATIC_HOOK(
    LevelTickHook,
    ll::memory::HookPriority::Normal,
    "?tick@Level@@UEAAXXZ",
    void*,
    void* self
){
    try {
		++tick;
		if (tick == resetTicks) {
			tick = 0;
			timeOld = timeNew;
			timeNew = getTimestamp();
		}
	} catch (const char*& err) {
		std::cout << err << std::endl;
	}
    return origin(self);
}