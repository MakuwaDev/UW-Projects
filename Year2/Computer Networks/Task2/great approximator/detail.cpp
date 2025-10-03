#include "detail.h"

#include <cerrno>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <limits>
#include <ifaddrs.h>
#include <signal.h>
#include <regex>
#include <fcntl.h>
#include <algorithm>
#include <cctype>

#include "err.h"

namespace detail {
    // Read a port number from a string.
    uint16_t read_port(std::string const& str) {
        try {
            int port = std::stoi(str);

            if (port <= 0 || port > std::numeric_limits<uint16_t>::max()) {
                fatal(str, " is not a valid port number.");
            }

            return static_cast<uint16_t>(port);
        } catch (const std::exception&) {
            fatal(str, " is not a valid port number.");
        }
    }

    // Convert a host and port to a sockaddr_storage structure.
    // Supports both IPv4 and IPv6.
    sockaddr_storage get_server_address(std::string const& host, uint16_t port, int af_hint) {
        // Set up the hints for getaddrinfo.
        addrinfo hints{};
        hints.ai_family = af_hint;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        
        // Get the real address.
        addrinfo* address_result = nullptr;
        int errcode = getaddrinfo(host.c_str(), nullptr, &hints, &address_result);
        if (errcode != 0) {
            fatal("getaddrinfo: ", gai_strerror(errcode), ".");
        }

        if (!address_result) {
            fatal("getaddrinfo: No address found for ", host, ".");
        }
        
        sockaddr_storage addr{};
        std::memcpy(&addr, address_result->ai_addr, address_result->ai_addrlen);

        if (addr.ss_family == AF_INET) {
            reinterpret_cast<sockaddr_in*>(&addr)->sin_port = htons(port);
        } else if (addr.ss_family == AF_INET6) {
            reinterpret_cast<sockaddr_in6*>(&addr)->sin6_port = htons(port);
        } else {
            freeaddrinfo(address_result);
            fatal("get_peer_address: Unsupported address family.");
        }
        
        freeaddrinfo(address_result);
        return addr;
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

    // Constructor for NodeClock
    Clock::Clock() : start_time(std::chrono::steady_clock::now()) {}

    // Method definition to get elapsed time in milliseconds
    int64_t Clock::get_time() const {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);
        return duration.count();
    }

    // Check if a player ID is valid.
    bool is_valid_player_id(std::string const& id) {
        static const std::regex pattern("^[A-Za-z0-9]+$");
        return std::regex_match(id, pattern);
    }

    // Extract the address and port from a sockaddr_storage structure.
    std::pair<std::string, uint16_t> get_representation(sockaddr_storage const& addr) {
        char ip_str[INET6_ADDRSTRLEN];
        uint16_t port;

        if (addr.ss_family == AF_INET) {
            sockaddr_in const* ipv4 = reinterpret_cast<sockaddr_in const*>(&addr);
            if (!inet_ntop(AF_INET, &ipv4->sin_addr, ip_str, sizeof(ip_str))) {
                fatal("inet_ntop.");
            }

            port = ntohs(ipv4->sin_port);
        } else {
            sockaddr_in6 const* ipv6 = reinterpret_cast<sockaddr_in6 const*>(&addr);
            if (!inet_ntop(AF_INET6, &ipv6->sin6_addr, ip_str, sizeof(ip_str))) {
                fatal("inet_ntop.");
            }
            
            port = ntohs(ipv6->sin6_port);
        }

        return {std::string(ip_str), port};
    }

    // Evaluate a polynomial at a given point x.
    double poly_val(std::vector<double> const& coeffs, int x) {
        double result = 0.0;
        double pow = 1.0;

        for (auto const& coeff : coeffs) {
            result += coeff * pow;
            pow *= x;
        }

        return result;
    }

    // Create and bind a socket to the specified port (host order).
    // Defaults to dual-stack, if IPv6 fails, it falls back to IPv4.
    // If port is 0, bind to OS assigned port.
    // Returns the socket file descriptor.
    // In case of fatal failure it will exit the program, so returned value is correct.
    int create_and_bind_socket(uint16_t port) {
        int sock = -1;
        bool ipv4_fallback = false; 
        
        sock = socket(AF_INET6, SOCK_STREAM, 0);
        if (sock >= 0) {
            int off = 0;
            if (setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &off, sizeof(off)) < 0) {
                syserr("setsockopt(IPV6_V6ONLY set to false) failed, falling back to IPv4.");
                close(sock);
                sock = -1;
                ipv4_fallback = true;
            } else {
                sockaddr_in6 addr6{};
                addr6.sin6_family = AF_INET6;
                addr6.sin6_addr = in6addr_any;
                addr6.sin6_port = htons(port);

                if (bind(sock, reinterpret_cast<sockaddr*>(&addr6), sizeof(addr6)) < 0) {
                    syserr("bind failed for IPv6, falling back to IPv4.");
                    close(sock);
                    sock = -1;
                    ipv4_fallback = true;
                }
            }
        } else {
            syserr("Failed to create IPv6 socket, falling back to IPv4.");
            ipv4_fallback = true;
        }

        if (ipv4_fallback) {
            sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) {
                fatal("Failed to create IPv4 socket.");
            }

            sockaddr_in addr4{};
            addr4.sin_family = AF_INET;
            addr4.sin_addr.s_addr = htonl(INADDR_ANY);
            addr4.sin_port = htons(port);

            if (bind(sock, reinterpret_cast<sockaddr*>(&addr4), sizeof(addr4)) < 0) {
                fatal("Failed to bind IPv4 socket.");
            }
        }

        return sock;
    }

    // Extract the real port number (host order) from a socket.
    uint16_t get_real_port(int sock) {
        sockaddr_storage addr;
        socklen_t addr_len = sizeof(addr);
        
        if (getsockname(sock, reinterpret_cast<sockaddr*>(&addr), &addr_len) < 0) {
            fatal("getsockname failed.");
        }

        if (addr.ss_family == AF_INET) {
            return ntohs(reinterpret_cast<sockaddr_in*>(&addr)->sin_port);
        } else if (addr.ss_family == AF_INET6) {
            return ntohs(reinterpret_cast<sockaddr_in6*>(&addr)->sin6_port);
        } else {
            fatal("Unsupported address family in getsockname.");
        }
    }

    // Get the number of lowercase letters in a string.
    int count_lowercase(const std::string& s) {
        return std::count_if(s.begin(), s.end(), [](unsigned char c) {
            return std::islower(c);
        });
    }
}   // namespace detail