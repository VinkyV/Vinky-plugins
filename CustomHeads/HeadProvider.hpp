#pragma once
#include <llapi/mc/BinaryStream.hpp>
#include <llapi/mc/Player.hpp>

void spawnFakePlayer(Player* player, Vec3 pos) {
	BinaryStream wp;

	wp.writeType(mce::UUID::fromString(id)); //UUID
	wp.writeString(""); //UserName
	wp.writeUnsignedVarInt64(-64424529358); // ActorId
	wp.writeString(""); //PlatformChatId
	wp.writeType(pos); //Position
	wp.writeType(Vec3{ 0.0, 0.0, 0.0 }); //Motion
	wp.writeType(Vec3{ 0.0, 0.0, 0.0 }); //Pitch, Yaw, HeadYaw
	wp.writeBool(false); //item
	wp.writeVarInt(1); //Gamemode
}