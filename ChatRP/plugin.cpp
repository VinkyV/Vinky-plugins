#include <iostream>
#include <llapi/EventAPI.h>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/Level.hpp>
#include <llapi/RegCommandAPI.h>
#include <Nlohmann/json.hpp>
#include <llapi/mc/SerializedSkin.hpp>
#include <llapi/FormUI.h>

#pragma comment(lib, "../SDK/lib/bedrock_server_api.lib")
#pragma comment(lib, "../SDK/lib/bedrock_server_var.lib")
#pragma comment(lib, "../SDK/lib/SymDBHelper.lib")
#pragma comment(lib, "../SDK/lib/LiteLoader.lib")

nlohmann::json data;

class Reply : public Command {
	string msg;
public:
	void execute(CommandOrigin const& ori, CommandOutput& outp) const override {
		if (ori.getPlayer()) {
			Player* player = ori.getPlayer();
			if (data.contains(player->getRealName())) {
				player->runcmd("w " + data[player->getRealName()].get<string>() + " " + msg);
			}
		}
		else {
			outp.error("No targets matched selector");
		}
	}

	static void setup(CommandRegistry* registry) {
		using RegisterCommandHelper::makeMandatory;
		registry->registerCommand("r", "commands.tell.description", CommandPermissionLevel::Any, { (CommandFlagValue)0 },
			{ (CommandFlagValue)0x80 });
		registry->registerOverload<Reply>("r", makeMandatory(&Reply::msg, "message"));
	}
};

class Try : public Command {
	string msg;
public:
	void execute(CommandOrigin const& ori, CommandOutput& outp) const override {
		if (ori.getPlayer()) {
			Player* player = ori.getPlayer();
			if (data.contains(player->getRealName())) {
				player->runcmd("w " + data[player->getRealName()].get<string>() + " " + msg);
			}
			Level::broadcastText(player->getRealName()+"", TextType::RAW);
		}
		else {
			outp.error("No targets matched selector");
		}
	}

	static void setup(CommandRegistry* registry) {
		using RegisterCommandHelper::makeMandatory;
		registry->registerCommand("try", "commands.me.description", CommandPermissionLevel::Any, { (CommandFlagValue)0 },
			{ (CommandFlagValue)0x80 });
		registry->registerOverload<Try>("r", makeMandatory(&Try::msg, "message"));
	}
};

void Main() {
	Event::PlayerCmdEvent::subscribe([](const Event::PlayerCmdEvent& ev) {
		std::stringstream ss(ev.mCommand);
		std::string s;
		std::vector<std::string> out;
		try {
			while (std::getline(ss, s, ' ')) {
				out.push_back(s);
			}
		} catch(std::string err) { }
		if (out[0] == "w" || out[0] == "tell" || out[0] == "msg") {
			data[ev.mPlayer->getRealName()] = out[1];
			data[out[1]] = ev.mPlayer->getRealName();
		}
		return true;
	});
	Event::PlayerLeftEvent::subscribe([](const Event::PlayerLeftEvent& ev) {
		if (data.contains(ev.mPlayer->getRealName())) {
			data.erase(ev.mPlayer->getRealName());
		}
		return true;
	});
	Event::RegCmdEvent::subscribe([](const Event::RegCmdEvent& e) {
		Reply::setup(e.mCommandRegistry);
		return true;
	});
}

THook(bool, "?isTrustedSkin@SerializedSkin@@QEBA_NXZ", SerializedSkin* skin) {
	return true;
}