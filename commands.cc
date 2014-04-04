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
const double REPLY_PROBABILITY = 0.3;

typedef std::function<Response(const std::vector<std::string>&)> CommandFn;

// TODO: persistant db
// TODO: multiple values
std::map<std::string, CommandFn> commands = { };

Response learn_fn(const std::vector<std::string>& parts) {
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

  commands[key] =
    [type, &value](const std::vector<std::string>&) -> Response {
      // TODO: pattern matching/replacement.
      return { type, value };
    };
  return { Response::TYPE_REPLY,
           "I learned that '" + key + "' is '" + value + "'" };
}

Response forget_fn(const std::vector<std::string>& parts) {
  std::vector<std::string> value(parts.begin() + 2, parts.end());
  std::string key = strings::join(value, " ");
  if (commands.count(key) == 0)
    return { Response::TYPE_REPLY,
             "I don't know \"" + key + "\" to forget it." };

  // TODO: determine which to forget in the case of multiple values.
  commands.erase(key);
  return { Response::TYPE_REPLY, "I forgot how to \"" + key + "\"." };
}

std::map<std::string, CommandFn> meta_commands = {
  {
    "help",
    [](const std::vector<std::string>&) -> Response {
      return { Response::TYPE_REPLY,
               "learn {key} is [action|message|reply] {value}, "
               "forget {key}, status, help" };
    }
  },
  {
    "status",
    [](const std::vector<std::string>&) -> Response {
      std::stringstream ss;
      ss << "I know " << commands.size() << " commands.";
      return { Response::TYPE_REPLY, ss.str() };
    }
  },
  { "learn", learn_fn },
  { "forget", forget_fn }
};

}  // end namespace

extern std::string mynick;

// TODO: multi-line responses
// TODO: Add serialization.
Response do_command(const Request& request) {
  // Tokenize the message.
  // TODO: strip off punctuation.
  auto parts = strings::split(request.message, ' ');

  if (parts[0] == mynick) {
    if (meta_commands.count(parts[1]) != 0)
      return meta_commands.at(parts[1])(parts);
  }

  // Stay quiet sometimes.
  if ((rand() % 100) / 100.0 < REPLY_PROBABILITY)
    return {Response::TYPE_NULL, std::string()};

  // Find all possible replies given the message.
  std::vector<CommandFn> replies;
  for (const auto& kv : commands)
    if (std::find(parts.begin(), parts.end(), kv.first) != parts.end())
      replies.push_back(kv.second);

  if (!replies.empty()) {
    // Pick a random reply.
    std::vector<CommandFn>::iterator it(replies.begin());
    std::advance(it, rand() % replies.size());
    // TODO: replace patterns as necessary.
    return (*it)(parts);
  }

  return { Response::TYPE_NULL, std::string() };
}

}  // end namespace bot

