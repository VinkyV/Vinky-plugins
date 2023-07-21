#include <llapi/EventAPI.h>
#include <llapi/DynamicCommandAPI.h>
#include <fstream>
#include <llapi/mc/Packet.hpp>
#include <llapi/mc/BinaryStream.hpp>
#include <llapi/mc/MinecraftPackets.hpp>
#include <llapi/mc/Actor.hpp>
#include <llapi/mc/Player.hpp>
#include "SkinPro.hpp"
#include "uuid.h"
#include "lodepng.h"
#include <llapi/mc/Level.hpp>
#include <llapi/RegCommandAPI.h>
#include <llapi/mc/BlockInstance.hpp>
#include <llapi/mc/Block.hpp>
#include <llapi/mc/BlockSource.hpp>
#include <llapi/mc/SynchedActorData.hpp>
#include <llapi/mc/SkullBlockActor.hpp>
#include <llapi/mc/DataItem2.hpp>

#pragma comment(lib, "../SDK/lib/bedrock_server_api.lib")
#pragma comment(lib, "../SDK/lib/bedrock_server_var.lib")
#pragma comment(lib, "../SDK/lib/SymDBHelper.lib")
#pragma comment(lib, "../SDK/lib/LiteLoader.lib")

void spawnFakePlayer(Player* player, Vec3 pos, double yaw) {
    string id = generate_random_id(32);
    string name = player->getRealName();
    BinaryStream wp;

    wp.writeType(mce::UUID::fromString(id)); //UUID
    wp.writeString(""); //UserName
    wp.writeUnsignedVarInt(-64424529358); // RuntimeId
    wp.writeString(""); //PlatformChatId

    wp.writeType(pos); //Position
    wp.writeType(Vec3{ 0.0, 0.0, 0.0 }); //Motion
    wp.writeType(Vec3{ 0.0, yaw, yaw }); //Pitch, Yaw, HeadYaw

    wp.writeBool(false); //item
    wp.writeVarInt(1); //Gamemode
    //MetaData(SynchedActorData)
    std::vector<std::unique_ptr<DataItem>> mItemsArray;
    mItemsArray.push_back(DataItem::create<float>(53, 0.001f)); //BOUNDING_BOX_WIDTH
    mItemsArray.push_back(DataItem::create<float>(54, 0.001f)); //BOUNDING_BOX_HEIGHT
    
    wp.writeType(mItemsArray); //MetaData

    wp.writeUnsignedVarInt(0);
    wp.writeUnsignedVarInt(0);

    wp.writeSignedInt64(123); //ActorId

    wp.writeByte(0);
    wp.writeByte(1);

    wp.writeByte(0);

    wp.writeUnsignedVarInt(0);

    wp.writeString(""); //DeviceId
    wp.writeSignedInt(0);
    std::shared_ptr<Packet> pkt = MinecraftPackets::createPacket(MinecraftPacketIds::AddPlayer);
    pkt->read(wp);
    Global<Level>->sendPacketForAllPlayers(*pkt);
    //Building PlayerSkinPacket
    auto pkt1 = MinecraftPackets::createPacket(MinecraftPacketIds::PlayerSkin);
    BinaryStream stream;
    stream.writeType(mce::UUID::fromString(id));

    SerializedSkin skin;
    skin.id = id;
    skin.skinid = id + "." + name;
    skin.playfabid = id + "." + name;
    skin.geometry = "{\"geometry\":{\"default\":\"geometry.humanoid.customskull\"}}";
    skin.geometryname = "geometry.humanoid.customskull";
    //ReadGeometry
    std::ifstream fin("plugins/skins/geometry.humanoid.customskull.json");
    Json::Reader reader;
    Json::Value geo;
    reader.parse(fin, geo);
    fin.close();
    //ReadSkinimage
    std::vector<unsigned char> decode;
    unsigned width, height;
    skin.skin.blob.bytes = new BYTE[16384];
    unsigned error = lodepng::decode(decode, width, height, "plugins/skins/"+name+".png");
    if (error)
        std::cout << "[SkinRead] " << error << std::endl;
    skin.skin.blob.size = 16384;
    memcpy(skin.skin.blob.bytes, decode.data(), decode.size());
    skin.skin.width = width;
    skin.skin.height = height;

    skin.cape.blob.bytes = new BYTE[0];
    skin.cape.blob.size = 0;
    skin.cape.height = 0;
    skin.cape.width = 0;
    skin.capeGeometry = "";
    skin.capeid = "";

    skin.skinGeometry = geo;
    skin.write(stream);
    stream.writeString(skin.getName());
    stream.writeString("Steve");
    stream.writeBool(true);
    pkt1->read(stream);
    Global<Level>->sendPacketForAllPlayers(*pkt1);
}

void despawnFakePlayer(Player* player) {
    BinaryStream wp;
    wp.writeUnsignedVarInt64(-64424529358);
    std::shared_ptr<Packet> pkt = MinecraftPackets::createPacket(MinecraftPacketIds::RemoveEntity);
    pkt->read(wp);
    player->sendNetworkPacket(*pkt);
}

class TestCommand : public Command {
    CommandPosition dpos;
public:
    void execute(CommandOrigin const& ori, CommandOutput& outp) const {
        spawnFakePlayer(ori.getPlayer(), dpos.getPosition(0,ori, Vec3(0.00, 0.00, 0.00)), 0.0);
    }

    static void setup(CommandRegistry* registry) {
        using RegisterCommandHelper::makeMandatory;
        using RegisterCommandHelper::makeOptional;
        registry->registerCommand("test", "Test Command", CommandPermissionLevel::GameMasters, { (CommandFlagValue)0 }, { (CommandFlagValue)0x80 });

        registry->registerOverload<TestCommand>("test", makeMandatory(&TestCommand::dpos, "position"));
    }
};

THook(void, "?onPlace@SkullBlock@@UEBAXAEAVBlockSource@@AEBVBlockPos@@@Z",void* blk, BlockSource* srcblk, BlockPos pos) {
    //srcblk->getBlockEntity(pos)->setCustomName("Test");
    //std::cout << srcblk->getBlockEntity(pos)->getCustomName() << std::endl;
    //std::cout << srcblk->getBlockEntity(pos)->getNbt()->toSNBT() << std::endl;
    //std::cout << pos.toString() << std::endl;
    //Player* pl = Global<Level>->getPlayer("VinkW");
    //spawnFakePlayer(pl, Vec3(double(pos.x) + 0.5, double(pos.y), double(pos.z) + 0.5), 0.0);
}

THook(void, "?setRotation@SkullBlockActor@@QEAAXM@Z", SkullBlockActor* blk, float rotation) {
    std::cout << "[setRotation] " << rotation << std::endl;
    BlockPos pos = blk->getPosition();
    Player* pl = Global<Level>->getPlayer("VinkW");
    spawnFakePlayer(pl, Vec3(double(pos.x) + 0.5, double(pos.y), double(pos.z) + 0.5), double(rotation)+180);
    return original(blk, rotation);
}

void PluginInit() {
    Event::RegCmdEvent::subscribe([&](Event::RegCmdEvent ev) {
        TestCommand::setup(ev.mCommandRegistry);
        return true;
    });
}