#include "commands.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>

#include "strings.h"

namespace bot {
namespace {
const double REPLY_PROBABILITY = 0.5;
}  // end namespace

extern std::string mynick;

// TODO: map string to function?
// TODO: persistant db
// TODO: multiple values
std::map<std::string, Response> commands = { };

// TODO: multi-line responses
// TODO: Add serialization.
Response do_command(const Request& request) {
  // Tokenize the message.
  // TODO: strip off punctuation.
  auto parts = strings::split(request.message, ' ');

  if (parts[0] == mynick) {
    if (parts[1] == "help") {
      return { Response::TYPE_REPLY,
               "learn {key} is [action|message|reply] {value}, "
               "forget {key}, status, help" };
    } else if (parts[1] == "learn") {
      std::string key = parts[2];
      size_t i = 3;
      while (parts[i] != "is" && i < parts.size())
        key += " " + parts[i++];

      // Skip the "is"
      ++i;

      Response::Type type = Response::TYPE_NULL;
      if (i < parts.size()) {
        if (parts[i] == "action")
          type = Response::TYPE_ACTION;
        else if (parts[i] == "message")
          type = Response::TYPE_MESSAGE;
        else if (parts[i] == "reply")
          type = Response::TYPE_REPLY;
        else
          return { Response::TYPE_REPLY,
                   "is that an action, a reply, or a message?" };

        // Skip the type.
        ++i;
      }

      std::string value;
      if (i < parts.size()) {
        value = parts[i++];
        for (; i < parts.size(); ++i)
          value += " " + parts[i];
      }

      if (key.empty())
        return { Response::TYPE_REPLY, "learn what?" };

      if (value.empty())
        return { Response::TYPE_REPLY,
                 "learn that '" + key + "' is what?" };

      commands[key] = {type, value};
      return { Response::TYPE_REPLY,
               "I learned that '" + key + "' is '" + value + "'" };
    } else if (parts[1] == "forget") {
      std::vector<std::string> value(parts.begin() + 2, parts.end());
      std::string key = strings::join(value, " ");
      if (commands.count(key) == 0)
        return { Response::TYPE_REPLY,
                 "I don't know \"" + key + "\" to forget it." };

      commands.erase(key);
      return { Response::TYPE_REPLY, "I forgot how to \"" + key + "\"." };
    } else if (parts[1] == "status") {
      std::stringstream ss;
      ss << "I know " << commands.size() << " commands.";
      return { Response::TYPE_REPLY, ss.str() };
    }
  } 

  // Stay quiet sometimes.
  if ((rand() % 100) / 100.0 < REPLY_PROBABILITY)
    return {};

  // Find all possible replies given the message.
  std::set<Response> replies;
  for (const auto& kv : commands)
    if (std::find(parts.begin(), parts.end(), kv.first) != parts.end())
      replies.insert(kv.second);

  if (!replies.empty()) {
    // Pick a random reply.
    std::set<Response>::const_iterator it(replies.begin());
    std::advance(it, rand() % replies.size());
    // TODO: replace patterns as necessary.
    return *it;
  }

  return {};
}

}  // end namespace bot

