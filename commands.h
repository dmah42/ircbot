#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include <string>

namespace bot {

struct Request {
  const std::string nick;
  const std::string user;
  const std::string server;
  const std::string channel;
  const std::string message;
};

struct Response {
  enum Type {
    TYPE_NULL,
    TYPE_REPLY,
    TYPE_ACTION,
    TYPE_MESSAGE,
  };

  Response() {}
  Response(Type type, const std::string &message)
      : type(type), message(message) {}

  bool operator < (const Response& r) const {
    if (type == r.type)
      return message < r.message;
    return type < r.type;
  }

  Type type = TYPE_NULL;
  std::string message;
};

Response do_command(const Request& request);

}  // end namespace bot

#endif  // _COMMANDS_H_
