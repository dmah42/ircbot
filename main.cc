#include <string.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <string>

#include "bot.h"
#include "strings.h"

namespace {
const char NICK[] = "mrdont";

// TODO: map string to function?
std::map<std::string, std::string> commands = {
  { "say {1}", "{nick}: {1}" },
  { "begone!", "/quit" },
  { "help", "{nick}: learn {key} is {value}"}
};

std::string handler(const std::string &nick, const std::string &user,
                    const std::string &server, const std::string &channel,
                    const std::string &message) {
  // Tokenize the message
  auto parts = strings::split(message, ' ');

  // Assume we must be the first token.
  if (parts[0] != NICK) return std::string();

  if (parts[1] == "learn") {
    std::string key = parts[2];
    size_t i = 3;
    while (parts[i] != "is" && i < parts.size())
      key += " " + parts[i++];

    std::string value;
    // Skip the "is"
    if (i++ < parts.size()) {
      value = parts[i++];
      for (; i < parts.size(); ++i)
        value += " " + parts[i];
    }

    if (key.empty())
      return nick + ": learn what?";

    if (value.empty())
      return nick + ": learn that '" + key + "' is what?";

    commands[key] = value;
    return nick + ": I learned that '" + key + "' is '" + value + "'";
  }

  for (const auto& kv : commands)
    if (kv.first == parts[1])
      return kv.second;

  return std::string();
}
}

int main(int argc, char* argv[]) {
  bot::handle_func = handler;
  bot::run(argv[1], NICK, {argv[2]});
  return 0;
}
