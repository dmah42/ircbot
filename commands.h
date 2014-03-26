#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include <string>

namespace bot {

std::string do_command(const std::string &nick, const std::string &user,
                       const std::string &server, const std::string &channel,
                       const std::string &message);

}  // end namespace bot

#endif  // _COMMANDS_H_
