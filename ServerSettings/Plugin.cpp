#include <iostream>
#include <fstream>
#include <string>
#include <llapi/mc/BinaryStream.hpp>
#include <llapi/mc/NetworkIdentifier.hpp>
#include <llapi/mc/MinecraftPackets.hpp>
#include <llapi/mc/Packet.hpp>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/ServerNetworkHandler.hpp>
#include <llapi/ScheduleAPI.h>

#pragma comment(lib, "../SDK/lib/bedrock_server_api.lib")
#pragma comment(lib, "../SDK/lib/bedrock_server_var.lib")
#pragma comment(lib, "../SDK/lib/SymDBHelper.lib")
#pragma comment(lib, "../SDK/lib/LiteLoader.lib")

using std::cout;
using std::endl;

std::string data = "";

void InitMain() {
    std::ifstream fs;
    fs.open("plugins/ServerSettings/config.json", std::ios::in);
    if (fs) {
        char buf[1024];
        while (fs.getline(buf, 1024)) {
            data.append(buf);
        }
    }
    else {
        cout << "[ServerSettings] Failed to read config.json!" << endl;
    }
}

THook(void, "?handle@?$PacketHandlerDispatcherInstance@VServerSettingsRequestPacket@@$0A@@@UEBAXAEBVNetworkIdentifier@@AEAVNetEventCallback@@AEAV?$shared_ptr@VPacket@@@std@@@Z", NetworkIdentifier* ni, ServerNetworkHandler* snh, void* packet) {
    BinaryStream wp;
    wp.writeUnsignedVarInt64(0);
    wp.writeString(data);
    std::shared_ptr<Packet> pkt = MinecraftPackets::createPacket(MinecraftPacketIds::ServerSettingsResponse);
    pkt->read(wp);
    ServerPlayer* player = snh->getServerPlayer(*ni, 0);
    player
    Schedule::delay([player] {
        BinaryStream wp;
        wp.writeUnsignedVarInt64(0);
        wp.writeString(data);
        std::shared_ptr<Packet> pkt = MinecraftPackets::createPacket(MinecraftPacketIds::ServerSettingsResponse);
        pkt->read(wp);
        player->se
    }, 20);
    cout << "Requested ServerSettings" << endl;
    return original(ni, snh, packet);
}