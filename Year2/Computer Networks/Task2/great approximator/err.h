#ifndef APPROX_ERR_H
#define APPROX_ERR_H

#include <string>
#include <cerrno>
#include <iostream>
#include <sstream>
#include <cstring>
#include <arpa/inet.h>

namespace err_util{
    // Utility function to convert multiple arguments to a string.
    template <typename... Args>
    std::string to_string(Args&&... args) {
        std::ostringstream oss;
        (oss << ... << std::forward<Args>(args));
        return oss.str();
    }
} // namespace err_util

// Print an error message.
// This function is used to print error messages that are not related to system calls.
template <typename... Args>
void err(Args&&... args) {
    std::cerr << "ERROR: " << err_util::to_string(std::forward<Args>(args)...) << "\n";
}

// Print an error message and exit the program.
// This function is used to print error messages that are not related to system calls.
template <typename... Args>
[[noreturn]] void fatal(Args&&... args) {
    err(std::forward<Args>(args)...);
    std::exit(EXIT_FAILURE);
}

// Print an error message with system error information and exit the program.
// This function is used to print error messages that are related to system calls.
template <typename... Args>
void syserr(Args&&... args) {
    int org_errno = errno;
    std::cerr << "ERROR: " << err_util::to_string(std::forward<Args>(args)...)
              << " (" << org_errno << "; " << std::strerror(org_errno) << ")\n";
}

// Print an error message related to message processing.
void msg_error(std::string const& ip_str, uint16_t port, std::string const& message, std::string const& player_id);

#endif // APPROX_ERR_H