#ifndef PEER_DETAIL_H
#define PEER_DETAIL_H

#include <cstdint>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <chrono>

namespace detail {
    // Read a port number from a string.
    uint16_t read_port(std::string const& str);

    // Convert a bind address and port to a sockaddr_in structure.
    // Empty host is replaced with INADDR_ANY.
    // Zero port is replaced with a system-assigned port.
    sockaddr_in get_bind_address(std::string const& str, uint16_t port);

    // Convert a host and port to a sockaddr_in structure.
    // The host can be an IP address or a hostname.
    sockaddr_in get_peer_address(std::string const& str, uint16_t port);

    // Check if a string is a valid IPv4 address.
    bool is_valid_ipv4(std::string const& str);

    // Check if a bind address is a local IP address.
    bool is_local_ip(std::string const& ip);

    // Install a signal handler for a specific signal.
    void install_signal_handler(int signal, void (*handler)(int), int flags);

    // Compare two sockaddr_in structures.
    bool is_same_sockaddr_in(sockaddr_in const& a, sockaddr_in const& b);

    // Check if other sockaddr_in represents this address.
    // Includes iterating through all interfaces if address is set to INADDR_ANY
    bool is_me(sockaddr_in const* me, sockaddr_in const* other);

    // Simple clock class to measure time.
    class NodeClock {
        public:
            NodeClock();                // Constructor.
            int64_t get_time() const;   // Get the elapsed time in milliseconds.
        
        private:
            std::chrono::steady_clock::time_point start_time;
    };

    // Comparator for sockaddr_in structures.
    // Used for std::set and std::map.
    struct sockaddr_in_cmp {
        bool operator()(const sockaddr_in& a, const sockaddr_in& b) const;
    };
}
#endif // PEER_DETAIL_H