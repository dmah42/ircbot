#include <string.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>

#include "bot.h"
#include "strings.h"

namespace {
const char NICK[] = "mrdont";
const double REPLY_PROBABILITY = 0.5;

// TODO: map string to function?
// TODO: persistant db
// TODO: multiple values
std::map<std::string, std::string> commands = { };

std::string handler(const std::string &nick, const std::string &user,
                    const std::string &server, const std::string &channel,
                    const std::string &message) {
  // Tokenize the message.
  // TODO: strip off punctuation.
  auto parts = strings::split(message, ' ');

  if (parts[0] == NICK ) {
    if (parts[1] == "help") {
      return nick + ": learn {key} is {value}, status, help";
    } else if (parts[1] == "learn") {
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
    } else if (parts[1] == "status") {
      std::stringstream ss;
      ss << nick << ": I know " << commands.size() << " commands.";
      return ss.str();
    }
  } 

  // Stay quiet sometimes.
  if ((rand() % 100) / 100.0 < REPLY_PROBABILITY)
    return std::string();

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
