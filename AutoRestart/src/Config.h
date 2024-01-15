#include <string>

struct Config {
  int version = 1;
  int hours = 23;
  int min = 59;
  std::string backupCmd = "backup";
};