#include <llapi/LoggerAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/mc/BinaryStream.hpp>
#include <llapi/mc/ChangeDimensionPacket.hpp>
#include <llapi/mc/MinecraftPackets.hpp>
#include <llapi/mc/ServerPlayer.hpp>

Logger logger("TestPlugin");

#pragma comment(lib, "../SDK/lib/bedrock_server_api.lib")
#pragma comment(lib, "../SDK/lib/bedrock_server_var.lib")
#pragma comment(lib, "../SDK/lib/SymDBHelper.lib")
#pragma comment(lib, "../SDK/lib/LiteLoader.lib")

using std::cout;
using std::endl;

void PluginMain() {
	logger.info("Loaded!");
}
// RakNetServerLocator srv
// d1
// d2
// d3
// GameType gm
// Online d4
// MaxOnline d5
// d6
// d7
THook(void*, "?startAnnouncingServer@RakNetServerLocator@@UEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@0V?$NonOwnerPointer@VAppPlatform@@@Bedrock@@W4GameType@@HH_N3@Z", RakNetServerLocator* srvloc, std::string d1, std::string d2, void* d3, GameType* gm, int d4, int d5, bool d6, bool d7) {
	//d4 = -777;
	return original(srvloc, d1, d2, d3, gm, d4, d5, d6, d7);
}

THook(void*, "??0ChangeDimensionPacket@@QEAA@V?$AutomaticID@VDimension@@H@@VVec3@@_N@Z", ChangeDimensionPacket* pkt, int id, Vec3 pos, bool hz) {
	cout << "ChangeDimension" << endl;
	cout << id << endl;
	if (id == 1) {
		id = 2;
	}
	return original(pkt, id, pos, hz);
}

THook(void*, "?handle@NetEventCallback@@UEAAXAEBVNetworkIdentifier@@AEBVChangeDimensionPacket@@@Z", ServerNetworkHandler* snh, NetworkIdentifier* ni, ChangeDimensionPacket* pkt) {
	try {
		ServerPlayer* pl = pkt->getPlayerFromPacket(snh, ni);
		BinaryStream bs;
		bs.writeUnsignedInt64(pl->getActorUniqueId().get());
		bs.writeSignedInt(14);
		bs.writeType(Vec3(0, 0, 0));
		bs.writeType(Vec3(0, 0, 0));
		bs.writeSignedInt(0);
		std::shared_ptr<Packet> packet = MinecraftPackets::createPacket(MinecraftPacketIds::PlayerAction);
		packet->write(bs); // Parse binary stream
		pl->sendNetworkPacket(*packet);
	}
	catch(string err) {
		cout << err << endl;
	}
	return original(snh, ni, pkt);
}