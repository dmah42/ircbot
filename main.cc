#include <iostream>
#include <string>
#include <vector>

#include "bot.h"

const char NICK[] = "mrdont";

int main(int argc, char* argv[]) {
  if (argc == 1) {
    std::cerr << "Usage: " << argv[0] << " <server> <channels>...\n";
    exit(1);
  }

  std::string server(argv[1]);
  std::vector<std::string> channels;
  for (size_t i = 2; i < argc; ++i)
    channels.push_back(argv[i]);

  bot::run(server, NICK, channels);
  return 0;
}
