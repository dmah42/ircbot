#include <string.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>

#include "bot.h"
#include "strings.h"

namespace {
const char NICK[] = "mrdont";

// TODO: map string to function?
// TODO: persistant db
// TODO: multiple values
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

  if (parts[0] == NICK && parts[1] == "learn") {
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

  // Find all possible replies given the message.
  std::set<std::string> replies;
  for (const auto& kv : commands)
    if (std::find(parts.begin(), parts.end(), kv.first) != parts.end())
      replies.insert(kv.second);

  if (!replies.empty()) {
    // Pick a random reply.
    std::set<std::string>::const_iterator it(replies.begin());
    std::advance(it, rand() % replies.size());
    // TODO: replace patterns as necessary.
    return *it;
  }

  return std::string();
}
}

int main(int argc, char* argv[]) {
  bot::handle_func = handler;
  bot::run(argv[1], NICK, {argv[2]});
  return 0;
}
