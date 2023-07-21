#include <random>
#include <llapi/EventAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/DynamicCommandAPI.h>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/ListTag.hpp>

#pragma comment(lib, "../SDK/lib/bedrock_server_api.lib")
#pragma comment(lib, "../SDK/lib/bedrock_server_var.lib")
#pragma comment(lib, "../SDK/lib/SymDBHelper.lib")
#pragma comment(lib, "../SDK/lib/LiteLoader.lib")

nlohmann::json data;

string loc(Player* player) {
	return "[" + std::to_string(player->getPosition().x) + " " + std::to_string(player->getPosition().y) + " " + std::to_string(player->getPosition().z) + "]";
}

class ReplyCommand : public Command {
	CommandMessage msg;
public:
	void execute(CommandOrigin const& ori, CommandOutput& outp) const override {
		Player* player = ori.getPlayer();
		if (player) {
			if (data.contains(player->getRealName())) {
				player->runcmd("w " + data[player->getRealName()].get<string>() + " " + msg.getMessage(ori));
			}
		}
		else {
			outp.error("This command for player's only!");
		}
	}

	static void setup(CommandRegistry* registry) {
		using RegisterCommandHelper::makeMandatory;
		registry->registerCommand("r", "commands.tell.description", CommandPermissionLevel::Any, { (CommandFlagValue)0 },
			{ (CommandFlagValue)0x80 });
		registry->registerAlias("r", "reply");
		registry->registerOverload<ReplyCommand>("r", makeMandatory(&ReplyCommand::msg, "message"));
	}
};

class TryCommand : public Command {
	CommandMessage msg;
public:
	void execute(CommandOrigin const& ori, CommandOutput& outp) const override {
		Player* player = ori.getPlayer();
		if (player) {
			string action = "";
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> dist(1, 2);
			if (dist(gen) == 1) {
				action = TextEncoding::fromUnicode(L"§aУспешно§f");
			}
			else {
				action = TextEncoding::fromUnicode(L"§cНеуспешно§f");
			}
			Level::broadcastText("* " + player->getRealName() + " " + msg.getMessage(ori) + " [" + action + "]", TextType::RAW);
		}
		else {
			outp.error("This command for player's only!");
		}
	}

	static void setup(CommandRegistry* registry) {
		using RegisterCommandHelper::makeMandatory;
		registry->registerCommand("try", TextEncoding::fromUnicode(L"Попытаться совершить действие").c_str(), CommandPermissionLevel::Any, {(CommandFlagValue)0},
			{ (CommandFlagValue)0x80 });
		registry->registerOverload<TryCommand>("try", makeMandatory(&TryCommand::msg, "message"));
	}
};

class RollCommand : public Command {
	int msg;
public:
	void execute(CommandOrigin const& ori, CommandOutput& outp) const override {
		Player* player = ori.getPlayer();
		if (player) {
			if (2 > msg || msg > 6) {
				player->sendText(TextEncoding::fromUnicode(L"§cЧисло должно быть больше 1 и не больше 6!"));
			}
			else {
				string action = "";
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<> dist(1, msg);
				Level::broadcastText("* " + player->getRealName() + TextEncoding::fromUnicode(L" выкинул ") + std::to_string(dist(gen)), TextType::RAW);
			}
		}
		else {
			outp.error("This command for player's only!");
		}
	}

	static void setup(CommandRegistry* registry) {
		using RegisterCommandHelper::makeMandatory;
		registry->registerCommand("roll", TextEncoding::fromUnicode(L"Подбросить игральные кости").c_str(), CommandPermissionLevel::Any, {(CommandFlagValue)0},
			{ (CommandFlagValue)0x80 });
		registry->registerOverload<RollCommand>("roll", makeMandatory(&RollCommand::msg, "count"));
	}
};

class IgnoreCommand : public Command {
	CommandSelector<Player> target;
public:
	void execute(CommandOrigin const& ori, CommandOutput& outp) const override {
		Player* player = ori.getPlayer();
		if (player) {
			//const ListTag* ignoreList = player->getNbt()->getListTag("ignore");
			player->sendText("Comming soon!");
		}
		else {
			outp.error("This command for player's only!");
		}
	}

	static void setup(CommandRegistry* registry) {
		using RegisterCommandHelper::makeMandatory;
		registry->registerCommand("ignore", TextEncoding::fromUnicode(L"Игнорировать сообщения игрока").c_str(), CommandPermissionLevel::Any, { (CommandFlagValue)0 },
			{ (CommandFlagValue)0x80 });
		registry->registerOverload<IgnoreCommand>("ignore", makeMandatory(&IgnoreCommand::target, "target"));
	}
};

class MuteCommand : public Command {
	CommandSelector<Player> target;
public:
	void execute(CommandOrigin const& ori, CommandOutput& outp) const override {
		Player* player = ori.getPlayer();
		string executor = "SERVER";
		if (player) {
			executor = player->getRealName();
		}
		Player* targetPlayer = Global<Level>->getPlayer(target.getName());
		if (targetPlayer->hasTag("muted")) {
			outp.error(TextEncoding::fromUnicode(L"Этот игрок уже в муте!"));
		}
		else {
			targetPlayer->addTag("muted");
			targetPlayer->sendText(TextEncoding::fromUnicode(L"§cВас замутили!"));
			Level::broadcastText(executor + TextEncoding::fromUnicode(L" §cзамутил игрока§f ") + target.getName(), TextType::RAW);
			outp.success(TextEncoding::fromUnicode(L"Вы замутили игрока!"));
		}
	}

	static void setup(CommandRegistry* registry) {
		using RegisterCommandHelper::makeMandatory;
		registry->registerCommand("mute", TextEncoding::fromUnicode(L"Замутить игрока").c_str(), CommandPermissionLevel::GameMasters, { (CommandFlagValue)0 },
			{ (CommandFlagValue)0x80 });
		registry->registerOverload<MuteCommand>("mute", makeMandatory(&MuteCommand::target, "target"));
	}
};

class UnmuteCommand : public Command {
	CommandSelector<Player> target;
public:
	void execute(CommandOrigin const& ori, CommandOutput& outp) const override {
		Player* player = ori.getPlayer();
		string executor = "SERVER";
		if (player) {
			executor = player->getRealName();
		}
		Player* targetPlayer = Global<Level>->getPlayer(target.getName());
		if (targetPlayer->hasTag("muted")) {
			targetPlayer->removeTag("muted");
			targetPlayer->sendText(TextEncoding::fromUnicode(L"§aВас размутили!"));
			Level::broadcastText(executor + TextEncoding::fromUnicode(L" §aразмутил игрока§f ") + target.getName(), TextType::RAW);
			outp.success(TextEncoding::fromUnicode(L"Вы размутили игрока!"));
		}
		else {
			outp.error(TextEncoding::fromUnicode(L"Этот игрок не в муте!"));
		}
	}

	static void setup(CommandRegistry* registry) {
		using RegisterCommandHelper::makeMandatory;
		registry->registerCommand("unmute", TextEncoding::fromUnicode(L"Размутить игрока").c_str(), CommandPermissionLevel::GameMasters, { (CommandFlagValue)0 },
			{ (CommandFlagValue)0x80 });
		registry->registerOverload<UnmuteCommand>("unmute", makeMandatory(&UnmuteCommand::target, "target"));
	}
};

bool onRegCmd(Event::RegCmdEvent event) {
	ReplyCommand::setup(event.mCommandRegistry);
	TryCommand::setup(event.mCommandRegistry);
	RollCommand::setup(event.mCommandRegistry);
	IgnoreCommand::setup(event.mCommandRegistry);
	MuteCommand::setup(event.mCommandRegistry);
	UnmuteCommand::setup(event.mCommandRegistry);
	return true;
}

bool onPlayerCmd(Event::PlayerCmdEvent event) {
	std::stringstream ss(event.mCommand);
	std::string s;
	std::vector<std::string> out;
	try {
		while (std::getline(ss, s, ' ')) {
			out.push_back(s);
		}
	}
	catch (std::string err) {}
	if (out[0] == "w" || out[0] == "tell" || out[0] == "msg") {
		data[event.mPlayer->getRealName()] = out[1];
		data[out[1]] = event.mPlayer->getRealName();
		Global<Level>->getPlayer(out[1])->sendPlaySoundPacket("random.orb", event.mPlayer->getPosition(), 1, 1);
	}
	return true;
}

bool onPlayerChat(Event::PlayerChatEvent event) {
	Player* player = event.mPlayer;
	if (player->hasTag("muted")) {
		player->sendText(TextEncoding::fromUnicode(L"§cВы в муте!"));
	}
	else {
		Level::broadcastText(player->getRealName() + " > " + event.mMessage, TextType::RAW);
	}
	return false;
}

bool onPlayerLeft(Event::PlayerLeftEvent event) {
	if (data.contains(event.mPlayer->getRealName())) {
		data.erase(event.mPlayer->getRealName());
	}
	return true;
}

void PluginMain() {
	Event::RegCmdEvent::subscribe(onRegCmd);
	Event::PlayerCmdEvent::subscribe(onPlayerCmd);
	//Event::PlayerChatEvent::subscribe(onPlayerChat);
	Event::PlayerLeftEvent::subscribe(onPlayerLeft);
}