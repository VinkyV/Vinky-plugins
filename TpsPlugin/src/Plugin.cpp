#include "Plugin.h"

#include <iostream>
#include <string>

#include <ll/api/memory/Hook.h>
#include <ll/api/memory/Memory.h>
#include <ll/api/service/Bedrock.h>
#include <ll/api/command/DynamicCommand.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <mc/world/level/Level.h>
#include <utility>
#include <sys/timeb.h>

int ticks = 0;
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
		return "[TPS]: §a" + output;
	}
	else if (input > 15.0) {
		return "[TPS]: §6" + output;
	}
	else return "[TPS]: §c" + output;
}

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {
    mSelf.getLogger().info("Loading...");
}

static void setupEchoCommand(CommandRegistry& registry) {
    auto command = DynamicCommand::createCommand(registry, "echo", "show message", CommandPermissionLevel::Any);
    command->addOverload(command->mandatory("text", DynamicCommand::ParameterType::RawText));
    command->setCallback([](DynamicCommand const&,
                            CommandOrigin const&,
                            CommandOutput&                                           output,
                            std::unordered_map<std::string, DynamicCommand::Result>& results) {
        auto text = results["text"].getRaw<std::string>();
        output.success(text);
    });
    DynamicCommand::setup(registry, std::move(command));
}

bool Plugin::enable() {
    auto& logger = mSelf.getLogger();

    auto commandRegistry = ll::service::getCommandRegistry();
    if (!commandRegistry) {
        throw std::runtime_error("failed to get command registry");
    }

    setupEchoCommand(commandRegistry);

    auto tpsCommand = DynamicCommand::createCommand(commandRegistry, "tps", "Show server tps", CommandPermissionLevel::Any);
    tpsCommand->addOverload();
    tpsCommand->setCallback(
        [](DynamicCommand const&, CommandOrigin const& origin, CommandOutput& output, std::unordered_map<std::string, DynamicCommand::Result>&) {
            output.success(getTps());
        }
    );
    DynamicCommand::setup(commandRegistry, std::move(tpsCommand));

    mSelf.getLogger().info("Loaded!");
    return true;
}

bool Plugin::disable() {
    auto commandRegistry = ll::service::getCommandRegistry();
    if (!commandRegistry) {
        throw std::runtime_error("failed to get command registry");
    }

    commandRegistry->unregisterCommand("tps");
    return true;
}

} // namespace plugin

// Hook Level tick func
LL_AUTO_TYPE_INSTANCE_HOOK(
    LevelTickingHook,
    ll::memory::HookPriority::Normal,
    Level,
    &Level::tick,
    void
){
    try {
		ticks++;
		if (ticks == resetTicks) {
			ticks = 0;
			timeOld = timeNew;
			timeNew = getTimestamp();
		}
	} catch (const char*& err) {
		std::cout << err << std::endl;
	}
    return origin();
}