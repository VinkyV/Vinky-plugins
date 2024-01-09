#include <string>

struct Config {
  int version = 1;
  std::string wsUri = "ws://localhost:1479/?sessionkey=123";
  std::string playerJoinMessage = "Player {} joined!";
  std::string playerChatMessage = "{}: {}";
  std::string playerLeaveMessage = "Player {} left!";
};