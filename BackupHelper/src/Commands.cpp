#include <mc/world/level/Command.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>

class BackupCommand : public Command {
public:
    void execute(CommandOrigin const& /*ori*/, CommandOutput& output) const override {
      output.success("123");
    }

    static void setup(CommandRegistry& registry) {
        registry.registerCommand("backup", "Backup server", CommandPermissionLevel::Any);
        registry.registerOverload<BackupCommand>("backup");
    }
};