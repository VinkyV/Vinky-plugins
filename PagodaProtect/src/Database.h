#include <sqlitecpp/SQLiteCpp.h>
#include <iostream>
#include <string>

#include <mc/world/actor/player/Player.h>

bool initDB();
std::string getUserName(int userId);
int getBlockId(std::string blockName);
bool logBlock(Player& player, int blockId, int dim, int x, int y, int z, int action, int tileData, int count = -1, int slot = -1, bool chest = false);