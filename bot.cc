#include "bot.h"

#include <assert.h>
#include <limits.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <iostream>

#include "commands.h"
#include "strings.h"

namespace bot {
namespace {

const char PORT[] = "6667";
const size_t MAX_DATA_SIZE = 128;
const char USER_NAME[] = "dmabot";

const char SERVER_NAME[] = "irc";
const char REAL_NAME[] = "dma bot";

int socket_fd = -1;

int create_socket(const std::string& server) {
  struct addrinfo hints, *servinfo;
  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(server.c_str(), PORT, &hints, &servinfo) != 0)
    return -1;

  int fd = socket(
      servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
  if (fd <= 0)
    return -1;

  if (connect(fd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
    close(fd);
    return -1;
  }

  freeaddrinfo(servinfo);
  return fd;
}

std::string time_now() {
  time_t raw_time;

  time(&raw_time);
  struct tm* timeinfo = localtime(&raw_time);

  char buffer[32];
  strftime(buffer, 32, "%F.%T", timeinfo);
  return std::string(buffer);
}

bool send_data(const std::string& data) {
  std::cout << time_now() << ">> Sending '" << data << "'\n";
  std::string message = data + "\r\n";
  return send(socket_fd, message.c_str(), message.size(), 0) != 0;
}

void handle(const std::string& message) {
  std::cout << time_now() << ">> " << message << '\n';

  // Parse the incoming message:
  // 2014-03-19.13:11:27>> :nrrd_!nrrd_@d.clients.kiwiirc.com PRIVMSG #haplessvictims :Glad to hear it
  // 2014-03-19.13:13:28>> PING :spectral.shadowcat.co.uk

  bool is_ping = message.substr(0, 5) == "PING ";
  if (is_ping) {
    send_data("PONG " + message.substr(5));
    return;
  }

  if (message[0] != ':') return;

  std::vector<std::string> parts = strings::split(message, ' ');

  // Only care about private messages
  if (parts[1] != "PRIVMSG") return;

  size_t nick_user_pos = parts[0].find_first_of('!');
  size_t user_server_pos = parts[0].find_first_of('@');

  std::string nick = parts[0].substr(1, nick_user_pos - 1);
  std::string user =
      parts[0].substr(nick_user_pos + 1, user_server_pos - nick_user_pos - 1);
  std::string server = parts[0].substr(user_server_pos + 1);
  std::string channel = parts[2];

  std::string inner_message = strings::join(
      std::vector<std::string>(parts.begin() + 3, parts.end()), " ").substr(1);

  Response response = do_command({nick, user, server, channel, inner_message});
  if (response.type != Response::TYPE_NULL) {
    std::string prefix, suffix;
    switch (response.type) {
      case Response::TYPE_REPLY:
        prefix = "PRIVMSG " + channel + " :" + nick + ": ";
        break;
        
      case Response::TYPE_ACTION:
        prefix = "PRIVMSG " + channel + " :\u0001ACTION ";
        suffix = "\u0001";
        break;

      case Response::TYPE_MESSAGE:
        prefix = "PRIVMSG " + channel + " :";
        break;

      case Response::TYPE_NULL:
        break;
    }
    send_data(prefix + response.message + suffix);
  }
}

}  // end namespace

std::string mynick;

void run(const std::string &server, const std::string &n,
         const std::vector<std::string> &channels) {
  mynick = n;

  socket_fd = create_socket(server);
  assert(socket_fd > 0);

  std::string buffer;

  int iterations = 0;
  while (true) {
    ++iterations;

    // TODO: respond to request for IDENT explicitly
    switch (iterations) {
      case 3:
        // send data to server (as per IRC protocol)
        char hostname[HOST_NAME_MAX];
        hostname[0] = '\0';
        assert(gethostname(hostname, HOST_NAME_MAX) == 0);
        assert(send_data("NICK " + nick));
        assert(send_data(
            "USER " + std::string(USER_NAME) + " " + std::string(hostname) +
            " " + std::string(SERVER_NAME) + " :" + std::string(REAL_NAME)));
        break;

      case 4:
        // join channels
        for (const std::string& channel : channels) {
          assert(send_data("JOIN " + channel));
        }
        break;

      default:
        break;
    }

    // receive and echo
    char recv_buffer[MAX_DATA_SIZE];
    size_t byte_count = recv(socket_fd, recv_buffer, MAX_DATA_SIZE-1, 0);
    if (byte_count == 0) {
      std::cout << time_now() << ">> Closing connection.\n";
      break;
    }

    std::string recv_buffer_str(recv_buffer, byte_count);

    size_t last_break_pos = 0U;
    size_t break_pos;
    while ((break_pos = recv_buffer_str.find("\r\n", last_break_pos)) !=
           std::string::npos) {
      buffer.append(
          recv_buffer_str.substr(last_break_pos, break_pos - last_break_pos));

      // pass to handler
      handle(buffer);
      buffer.clear();
      last_break_pos = break_pos + 2;  // 2 for "\r\n"
    }
    buffer.append(recv_buffer_str.substr(last_break_pos));
  }
  close(socket_fd);
}
}  // end namespace bot
