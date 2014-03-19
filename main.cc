#include "bot.h"

#include <string.h>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace {
const char NICK[] = "mrdont";

// TODO: split punctuation, etc
std::vector<std::string> split(const std::string& s) {
  std::stringstream ss(s);
  std::vector<std::string> items;
  std::string item;
  while (std::getline(ss, item, ' ')) {
    std::transform(item.begin(), item.end(), item.begin(), ::tolower);
    items.push_back(item);
  }
  return items;
}

std::string handler(const std::string &nick, const std::string &user,
                    const std::string &server, const std::string &channel,
                    const std::string &message) {
  // Tokenize the message
  auto parts = split(message);

  // Assume we must be the first token.
  if (parts[0] != NICK) return std::string();

  if (parts[1] == "say") {
    std::ostringstream os;
    std::copy(parts.begin() + 2, parts.end() - 1,
              std::ostream_iterator<std::string>(os, " "));
    os << *(parts.rbegin());
    return "PRIVMSG " + channel + " :" + os.str() + ", " + nick;
  }
  return std::string();
}
}

int main() {
  bot::handle_func = handler;
  bot::run("irc.perl.org", NICK, {"#haplessvictims"});
  return 0;
}
