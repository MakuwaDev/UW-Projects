#ifndef APPROX_DETAIL_H
#define APPROX_DETAIL_H

#include <cstdint>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <chrono>
#include <vector>

namespace detail {
    // Read a port number from a string.
    uint16_t read_port(std::string const& str);

    // Convert a host and port to a sockaddr_storage structure.
    // Supports both IPv4 and IPv6.
    sockaddr_storage get_server_address(std::string const& str, uint16_t port, int af_hint);

    // Install a signal handler for a specific signal.
    void install_signal_handler(int signal, void (*handler)(int), int flags);

    // Simple clock class to measure time.
    class Clock {
        public:
            Clock();                // Constructor.
            int64_t get_time() const;   // Get the elapsed time in milliseconds.
        
        private:
            std::chrono::steady_clock::time_point start_time;
    };

    // Check if a player ID is valid.
    bool is_valid_player_id(std::string const& str);

    // Get the address and port from a sockaddr_storage structure.
    std::pair<std::string, uint16_t> get_representation(sockaddr_storage const& addr);

    // Evaluate a polynomial at a given point x.
    double poly_val(std::vector<double> const& coeffs, int x);

    // Create and bind a socket to the specified port.
    // Defaults to dual-stack, if IPv6 fails, it falls back to IPv4.
    // If port is 0, bind to OS assigned port.
    // Returns the socket file descriptor.
    // In case of fatal failure it will exit the program, so returned value is correct.
    int create_and_bind_socket(uint16_t port);

    // Extract the real port number from a socket.
    uint16_t get_real_port(int sock);

    // Get the number of lowercase letters in a string.
    int count_lowercase(const std::string& s);
}
#endif // PEER_DETAIL_H