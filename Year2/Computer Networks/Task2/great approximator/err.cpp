#include "err.h"

#include <iomanip>
#include <sys/socket.h>
#include <arpa/inet.h>

// Print an error message related to message processing.
void msg_error(std::string const& ip_str, uint16_t port, std::string const& message, std::string const& player_id) {
    err("bad message from [", ip_str, "]:", port, ", ", player_id, ": ", message);
}