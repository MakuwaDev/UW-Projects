#include "detail.h"

#include <cerrno>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <limits>
#include <ifaddrs.h>
#include <signal.h>

#include "err.h"

namespace detail {
    constexpr std::string ALL_INTERFACES{"0.0.0.0"};

    // Helper function to check if addres is a local interface.
    // Used in is_local_ip and is_me.
    // Assumes network byte order.
    bool is_local_ip_raw(uint32_t addr) {
        ifaddrs* ifaddr;
        ifaddrs* ifa;
        bool found = false;

        if (getifaddrs(&ifaddr) == -1) {
            syserr("getifaddrs");
            std::exit(EXIT_FAILURE);
        }

        for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
            // Check only IPv4 addresses.
            if (ifa->ifa_addr == nullptr || ifa->ifa_addr->sa_family != AF_INET) {
                continue;
            }

            sockaddr_in* sa = reinterpret_cast<sockaddr_in*>(ifa->ifa_addr);

            if (sa->sin_addr.s_addr == addr) {
                found = true;
                break;
            }
        }

        freeifaddrs(ifaddr);
        return found;
    }

    // Read a port number from a string.
    uint16_t read_port(std::string const& str) {
        try {
            int port = std::stoi(str);

            if (port <= 0 || port > std::numeric_limits<uint16_t>::max()) {
                fatal(str, " is not a valid port number");
            }

            return static_cast<uint16_t>(port);
        } catch (const std::exception&) {
            fatal(str, " is not a valid port number");
        }
    }

    // Convert a bind address and port to a sockaddr_in structure.
    // Empty host is replaced with INADDR_ANY.
    // Zero port is replaced with a system-assigned port.
    sockaddr_in get_bind_address(std::string const& host, uint16_t port) {
        sockaddr_in bind_addr{};

        bind_addr.sin_family = AF_INET;

        // If the host is empty, bind to all interfaces.
        if (host.empty() || host == ALL_INTERFACES) {
            bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        } else {
            if (inet_pton(AF_INET, host.c_str(), &bind_addr.sin_addr) <= 0) {
                fatal("inet_pton: ", host.c_str());
            }
        }

        // Set the port.
        bind_addr.sin_port = htons(port);

        return bind_addr;
    }

    // Convert a host and port to a sockaddr_in structure.
    // The host can be an IP address or a hostname.
    sockaddr_in get_peer_address(std::string const& host, uint16_t port) {
        // Set up the hints for getaddrinfo.
        addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;
        
        // Get the real address.
        addrinfo* address_result;
        int errcode = getaddrinfo(host.c_str(), nullptr, &hints, &address_result);
        if (errcode != 0) {
            fatal("getaddrinfo: ", gai_strerror(errcode));
        }

        // Set up the sockaddr_in structure.
        sockaddr_in send_address{};
        send_address.sin_family = AF_INET;
        send_address.sin_addr.s_addr =
                reinterpret_cast<sockaddr_in*>((address_result->ai_addr))->sin_addr.s_addr;
        send_address.sin_port = htons(port);

        freeaddrinfo(address_result);

        return send_address;
    }

    // Check if a string is a valid IPv4 address
    bool is_valid_ipv4(std::string const& str) {
        sockaddr_in addr;
        return inet_pton(AF_INET, str.c_str(), &addr.sin_addr) == 1;
    }

    // Check if address is a local IP address.
    bool is_local_ip(std::string const& ip) {
        in_addr addr;
        if (inet_pton(AF_INET, ip.c_str(), &addr) != 1) {
            syserr("inet_pton");
            std::exit(EXIT_FAILURE);
        }

        return is_local_ip_raw(addr.s_addr);
    }

    // Install a signal handler for a specific signal.
    void install_signal_handler(int signal, void (*handler)(int), int flags) {
        struct sigaction action;
        sigset_t block_mask;
    
        sigemptyset(&block_mask);
        action.sa_handler = handler;
        action.sa_mask = block_mask;
        action.sa_flags = flags;
    
        if (sigaction(signal, &action, NULL) < 0) {
            syserr("sigaction");
        }
    }

    // Compare two sockaddr_in structures.
    bool is_same_sockaddr_in(sockaddr_in const& a, sockaddr_in const& b) {
        return a.sin_family == b.sin_family &&
               a.sin_addr.s_addr == b.sin_addr.s_addr &&
               a.sin_port == b.sin_port;
    }

    // Check if other sockaddr_in represents this address.
    // Includes iterating through all interfaces if address is set to INADDR_ANY
    bool is_me(sockaddr_in const* me, sockaddr_in const* other) {
        if (me->sin_addr.s_addr == htonl(INADDR_ANY)) {
            return is_local_ip_raw(other->sin_addr.s_addr) &&
                   me->sin_family == other->sin_family &&
                   me->sin_port == other->sin_port;
        } else {
            return is_same_sockaddr_in(*me, *other);
        }
    }

    // Constructor for NodeClock
    NodeClock::NodeClock() : start_time(std::chrono::steady_clock::now()) {}

    // Method definition to get elapsed time in milliseconds
    int64_t NodeClock::get_time() const {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);
        return duration.count();
    }

    // Comparator for sockaddr_in structures.
    // Used for std::set and std::map.
    bool sockaddr_in_cmp::operator()(const sockaddr_in& a, const sockaddr_in& b) const {
        if (a.sin_addr.s_addr != b.sin_addr.s_addr)
            return a.sin_addr.s_addr < b.sin_addr.s_addr;
        return a.sin_port < b.sin_port;
    }
}   // namespace detail