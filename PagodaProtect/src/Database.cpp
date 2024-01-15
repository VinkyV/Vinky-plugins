#include "Database.h"
#include <memory>

#include <fmt/format.h>

static std::unique_ptr<SQLite::Database> db;

bool initDB() {
  try {
    db = std::make_unique<SQLite::Database>("plugins/PagodaProtect/database.db", SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
    db->exec("PRAGMA journal_mode = MEMORY");
    db->exec("PRAGMA synchronous = NORMAL");
    db->exec("CREATE TABLE IF NOT EXISTS co_art_map (id INTEGER PRIMARY KEY AUTOINCREMENT, block TEXT UNIQUE)");
    db->exec("CREATE TABLE IF NOT EXISTS co_user (id INTEGER PRIMARY KEY AUTOINCREMENT, nickname TEXT UNIQUE, inspector INT DEFAULT 0, wid INT DEFAULT -1, x INT DEFAULT 0, y INT DEFAULT -256, z INT DEFAULT 0)");
    db->exec("CREATE TABLE IF NOT EXISTS co_block (time TIMESTAMP DEFAULT (CAST (((julianday('now') - 2440587.5) * 86400.0 * 1000) AS INT)), block INT, user INT, action INT, wid INT, x INT, y INT, z INT, data INT, count INT, slot INT, explode TEXT DEFAULT -1, rollback INT DEFAULT 0)");

  } catch (std::exception const& e) {
    std::cout << "Database error: " << e.what() << std::endl;
    return false;
  }
  return true;
}

int getUserId(std::string playerName) {
	int userId = -1;
  try {
    SQLite::Statement query(*db, "SELECT id FROM co_user WHERE nickname = ?");
    query.bind(1, playerName);
    while (query.executeStep()) {
      int plid = query.getColumn(0);
      userId = plid;
    }
  }
  catch (std::exception& e) {
    db->exec("INSERT INTO co_user (nickname) VALUES (\""+playerName+"\")");
  }
  try {
    SQLite::Statement query(*db, "SELECT id FROM co_user WHERE nickname = ?");
    query.bind(1, playerName);
    while (query.executeStep()) {
      int plid = query.getColumn(0);
      userId = plid;
    }
  }
  catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
	return userId;
}

std::string getUserName(int userId) {
  std::string name = "";
  try {
    SQLite::Statement query(*db, "SELECT nickname FROM co_user WHERE id = ?");
    query.bind(1, userId);
    while (query.executeStep()) {
      const char* plName = query.getColumn(0);
      name = std::string(plName);
    }
	  return name;
  }
  catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
}

int getBlockId(std::string blockName) {
  return 1;
}

bool logBlock(Player& player, int blockId, int dim, int x, int y, int z, int action, int tileData, int count, int slot, bool chest) {
  if (chest) {
    //db->exec("INSERT INTO co_block (block, user, action, wid, x, y, z, data, count, slot) VALUES ("+std::to_string(blockId)+", -1, "+std::to_string(action)+", "+std::to_string(dim)+", "+std::to_string(x)+", "+std::to_string(y)+", "+std::to_string(z)+", "+std::to_string(tileData)+", "+std::to_string(count)+", "+std::to_string(slot)+")");
    db->exec(fmt::format("INSERT INTO co_block (block, user, action, wid, x, y, z, data, count, slot) VALUES ({}, -1, {}, {}, {}, {}, {}, {}, {}, {})", blockId, action, dim, x, y, z, tileData, count, slot));
  }
  //db->exec(fmt::format("INSERT INTO co_block (block, user, action, wid, x, y, z, data, count, slot) VALUES (${getBlockId(blockName)}, ${getUserId(pl.realName)}, ${action}, ${wid}, ${x}, ${y}, ${z}, ${tileData}, ${count}, ${slot})", blockId));
}