#include <sqlitecpp/SQLiteCpp.h>
#include <iostream>

using std::cout;
using std::endl;

static std::unique_ptr<SQLite::Database> db;

struct cleanSTMT {
    SQLite::Statement& get;

    cleanSTMT(SQLite::Statement& g) : get(g) {}

    ~cleanSTMT() {
        get.reset();
        get.clearBindings();
    }
};

bool initDB() {
  try {
    db = std::make_unique<SQLite::Database>("plugins/PagodaProtect/database.db", SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
    db->exec("PRAGMA journal_mode = MEMORY");
    db->exec("PRAGMA synchronous = NORMAL");
    db->exec("CREATE TABLE IF NOT EXISTS co_art_map (id INTEGER PRIMARY KEY AUTOINCREMENT, block TEXT UNIQUE)");
    db->exec("CREATE TABLE IF NOT EXISTS co_user (id INTEGER PRIMARY KEY AUTOINCREMENT, nickname TEXT UNIQUE, inspector INT DEFAULT 0, wid INT DEFAULT -1, x INT DEFAULT 0, y INT DEFAULT -256, z INT DEFAULT 0)");
    db->exec("CREATE TABLE IF NOT EXISTS co_block (time TIMESTAMP DEFAULT (CAST (((julianday('now') - 2440587.5) * 86400.0 * 1000) AS INT)), block INT, user INT, action INT, wid INT, x INT, y INT, z INT, data INT, count INT, slot INT, explode TEXT DEFAULT -1, rollback INT DEFAULT 0)");

  } catch (std::exception const& e) {
    cout << "Database error: " << e.what() << endl;
    return false;
  }
  return true;
}

bool logBlock(Player player, std::string blockName, int dim, int x, int y, int z, int action, int tileData, int count = -1, int slot = -1, bool chest = false) {
  if (chest) {
    db->exec(`INSERT INTO co_block (block, user, action, wid, x, y, z, data, count, slot) VALUES (${getBlockId(blockName)}, -1, ${action}, ${wid}, ${x}, ${y}, ${z}, ${tileData}, ${count}, ${slot})`);
  }
}