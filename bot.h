#ifndef _BOT_H_
#define _BOT_H_

#include <functional>
#include <string>
#include <vector>

namespace bot {
extern std::function<std::string(
    const std::string &nick, const std::string &user, const std::string &server,
    const std::string &channel, const std::string &message)> handle_func;

void run(const std::string &server, const std::string &nick,
         const std::vector<std::string> &channels);
}  // namespace bot

#endif
