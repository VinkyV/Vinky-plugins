#include <iostream>
#include <time.h>
#include <sstream>
#include <string>
#include <sys/timeb.h>
#include <llapi/HookAPI.h>
#include <llapi/DynamicCommandAPI.h>
#include <llapi/mc/Packet.hpp>
#include <llapi/mc/ServerPlayer.hpp>
#include <llapi/EventAPI.h>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/ServerLevel.hpp>
#include <llapi/mc/Dimension.hpp>

#pragma comment(lib, "../SDK/lib/bedrock_server_api.lib")
#pragma comment(lib, "../SDK/lib/bedrock_server_var.lib")
#pragma comment(lib, "../SDK/lib/SymDBHelper.lib")
#pragma comment(lib, "../SDK/lib/LiteLoader.lib")

using std::cout;
using std::endl;

int tick = 0;
const float resetTicks = 20.0;
long timeOld = 0;
long timeNew = 0;

long getTimestamp() {
	timeb t;
	ftime(&t);
	return t.time * 1000 + t.millitm;
}

std::string getTps() {
    float input = 1000.0 * (resetTicks / (timeNew - timeOld));
    auto output = std::to_string(input);
    if (input >= 18.5) {
        return TextEncoding::fromUnicode(L"[§a/§f] TPS: §a") + output;
    }
    else if (input > 15.0) {
        return TextEncoding::fromUnicode(L"[§a/§f] TPS: §6") + output;
    }
    else return TextEncoding::fromUnicode(L"[§a/§f] TPS: §c") + output;
}

THook(void, "?tick@Level@@UEAAXXZ", void* self) {
    original(self);
    try {
        ++tick;
        if (tick == resetTicks) {
            tick = 0;
            timeOld = timeNew;
            timeNew = getTimestamp();
            //cout << time_old << " " << time_new << " " << time_new-time_old <<"\n";
        }
    } catch (const char*& e) {
        //ERROR(e);
    }
}

void TritiumMain() {
    int tick = 0;
    const float resetTicks = 20.0;
    long timeOld = 0;
    long timeNew = 0;
    cout << "[Tritium] Loaded!" << endl;
    DynamicCommand::setup("tps", "Show server's tps", {}, {}, { {}, }, [](DynamicCommand const& command, CommandOrigin const& origin, CommandOutput& output, std::unordered_map<std::string, DynamicCommand::Result>& results) {
        return output.success(getTps().c_str());
    }, CommandPermissionLevel::Any);
}