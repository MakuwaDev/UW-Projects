#include <string>
#include <getopt.h>
#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>
#include <map>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>
#include <signal.h>

#include "protocol.h"
#include "detail.h"
#include "err.h"

// Useful constants.
constexpr std::size_t BUFFER_SIZE = 1024;
constexpr int64_t DISCONNECT_TIMEOUT = 3000;
constexpr std::size_t CONNECT_QUEUE_SIZE = 32;

// Simple struct to hold console parameters.
struct Config {
    std::string file_name{};                    // COEFF file name.
    uint32_t m = 131;                           // PUT threshold.
    uint16_t k = 100;                           // Available PUT points.
    uint8_t n = 4;                              // Polynomial degree.
    uint16_t port = 0;                          // Server port.
};

// Simple struct to hold client info.
struct Client {
    sockaddr_storage addr{};                                    // Client addres;
    std::vector<double> coeffs{};                               // Client polynomial coeffs.
    std::vector<double> state{};                                // Client approximation state.
    std::multimap<int64_t, std::string> retarded_responses{};   // Awaiting client responses.
    std::string write_buffer{};                                 // Client write buffer.
    std::string read_buffer{};                                  // Client read buffer.
    std::string player_id{};                                    // Client player id.
    std::string ip_str{};                                       // Client ip address, for printing.
    uint16_t port;                                              // Client port, for printing.
    int64_t connect_time{};                                     // Client connection time.
    bool received_hello{};                                      // Flag whether client sent HELLO message.
    uint32_t sent_puts{};                                       // Count of correct PUTs sent by client.
    int64_t retardation{};                                      // Client response retardation.
    double score{};                                             // Client score.
};

// Flag to determine whether the program should continue running.
// Changed by catching SIGINT.
static bool running = true;

// Print the usage message and exit the program.
[[noreturn]] static void print_usage(char* progname) {
    fatal("Usage: ", progname, " [-p <port>] [-k <K>] [-n <N>] [-m <M>] -f <file>");
}

// Cancel the while loop after receiving a signal.
static void catch_int([[maybe_unused]] int) {
    running = false;
}

// Parse the program parameters.
Config parse_args(int argc, char* argv[]) {
    detail::Clock clock{};
    Config config{};

    bool p_given = false, k_given = false, n_given = false, m_given = false, f_given = false;

    opterr = 0;
    int opt;
    while ((opt = getopt(argc, argv, "p:k:n:m:f:")) != -1) {
        try {
            switch (opt) {
                case 'p':
                    if (p_given) {
                        print_usage(argv[0]);
                    }
                    
                    if (std::string(optarg) == "0") {
                        p_given = true;
                        break;
                    }

                    config.port = detail::read_port(optarg);
                    p_given = true;
                    break;

                case 'k':
                    if (k_given) {
                        print_usage(argv[0]);
                    }

                    config.k = std::stoi(optarg);
                    if (config.k < 1 || config.k > 10000) {
                        fatal("K must be between 1 and 10000");
                    }

                    k_given = true;
                    break;

                case 'n':
                    if (n_given) {
                        print_usage(argv[0]);
                    }
                    
                    config.n = static_cast<uint8_t>(std::stoi(optarg));
                    if (config.n < 1 || config.n > 8) {
                        fatal("N must be between 1 and 8");
                    }
                    
                    n_given = true;
                    break;
                
                case 'm':
                    if (m_given) {
                        print_usage(argv[0]);
                    }
                    
                    config.m = static_cast<uint32_t>(std::stoul(optarg));
                    if (config.m < 1 || config.m > 12341234) {
                        fatal("M must be between 1 and 12341234");
                    }
                    
                    m_given = true;
                    break;
                case 'f':
                    if (f_given) {
                        print_usage(argv[0]);
                    }

                    config.file_name = optarg;
                    f_given = true;
                    break;
                
                default:
                    print_usage(argv[0]);
            }
        } catch (std::exception const&) {
            fatal("Invalid argument for option:", std::string(1, static_cast<char>(opt)));
        }
    }

    if (!f_given) {
        fatal("-f option is required");
    } else if (optind < argc) {
        fatal("Unexpected positional argument: ", argv[optind]);
    }

    return config;
}

// Disconnect client and delete any information about him.
// Includes reverting puts submitted by client.
static void disconnect_client(std::vector<pollfd>& fds, std::size_t i,
                              std::map<int, Client>& client_map,
                              uint32_t& received_puts) {
    received_puts -= client_map[fds[i].fd].sent_puts;
    client_map.erase(fds[i].fd);
    close(fds[i].fd);
    fds[i].fd = -1;
}

// Print error message and change a referenced flag.
// Used basically as a C macro.
static void invalidate_message(Client& client, std::string const& message, bool& valid) {
    msg_error(client.ip_str, client.port, message, client.player_id);
    valid = false;
}

// Poll on sockets, return true if poll goes through.
// Exit fatally if poll fails, return false if it was interrupted.
static bool wait_on_poll(std::vector<pollfd>& fds, int64_t timeout) {
    int poll_result = poll(fds.data(), fds.size(), timeout);
    if (poll_result < 0) {
        if (errno == EINTR) {
            return false;
        }
        fatal("Poll failed.");
    }
    
    return true;
}

// Accept an incoming connection.
// Reuse empty spaces in the given vector, push back if none found.
static void accept_connection(Config const& config, std::vector<pollfd>& fds,
                              std::map<int, Client>& client_map, detail::Clock const& clock) {
    sockaddr_storage client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = accept(fds[0].fd,
                            reinterpret_cast<sockaddr*>(&client_addr),
                            &addr_len);

    if (client_fd < 0) {
        syserr("Failed to accept new connection");
    } else {
        bool found = false;
        for (std::size_t i = 1; i < fds.size(); ++i) {  // Try to reuse a pollfd structure.
            if (fds[i].fd < 0) {
                fds[i].fd = client_fd;
                found = true;
                break;
            }
        }

        // If no free pollfds, push a new one back.
        if (!found) {
            fds.push_back({client_fd, POLLIN | POLLOUT, 0});
        }

        auto [ip_str, port] = detail::get_representation(client_addr);
        std::cout << "New client [" << ip_str << "]:" << port << ".\n";

        // Set the new client parameters and add it to client map.
        Client new_client{};
        new_client.connect_time = clock.get_time();
        new_client.addr = client_addr;
        new_client.state.resize(config.k + 1, 0.0);
        std::tie(new_client.ip_str, new_client.port) = detail::get_representation(client_addr);
        client_map[client_fd] = std::move(new_client);
    }
}

// Check if poll detected any errors on given socket.
// If so, disconnect appropriate client.
// Returns true if no error occurred, false otherwise.
static bool check_poll_error(std::vector<pollfd>& fds, std::size_t i,
                             std::map<int, Client>& client_map,
                             uint32_t& received_puts) {

    Client& client = client_map[fds[i].fd];
    if (fds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) {
        std::cout << "Client [" << client.ip_str << "]:" << client.port << " disconnected.\n";
        disconnect_client(fds, i, client_map, received_puts);
        return false;
    }

    return true;
}

// Read from a socket.
// Closes the socket if errors occur.
// Returns false if the read results in a disconnect.
static bool receive_messages(std::vector<pollfd>& fds, std::size_t i,
                             std::map<int, Client>& client_map,
                             uint32_t& received_puts) {
    char buffer[BUFFER_SIZE];
    Client& client = client_map[fds[i].fd];

    ssize_t received = recv(fds[i].fd, buffer, sizeof(buffer), 0);
    if (received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return true;
        } else {
            syserr("Failed to receive data from [", client.ip_str,
                   "]:", client.port, ", closing connection.");
            disconnect_client(fds, i, client_map, received_puts);
            return false;
        }
    } else if (received == 0) {
        std::cout << "Client [" << client.ip_str << "]:" << client.port << " disconnected.\n";
        disconnect_client(fds, i, client_map, received_puts);
        return false;
    } else {
        client.read_buffer.append(buffer, received);
        return true;
    }
}

// Write to socket.
// Closes the socket if errors occur.
// Returns false if the write results in a disconnect.
static bool send_messages(std::vector<pollfd> fds, std::size_t i,
                          std::map<int, Client>& client_map,
                          uint32_t& received_puts) {
    Client& client = client_map[fds[i].fd];
    ssize_t sent = send(fds[i].fd, client.write_buffer.data(), client.write_buffer.size(), 0);
    if (sent < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            sent = 0;
            return false;
        } else {
            syserr("Failed to send data to [", client.ip_str, "]:", client.port, ", closing connection.");
            disconnect_client(fds, i, client_map, received_puts);
            return false;
        }
    } else {
        client.write_buffer.erase(0, sent);
        return true;
    }
}

// Add retarded messages to write buffer if they're eligible to be sent.
static void process_retarded_messages(detail::Clock const& clock, Client& client) {
    int64_t current_time = clock.get_time();

    // Iterate over retarded messages.
    for (auto it = client.retarded_responses.begin(); it != client.retarded_responses.end();) {
        if (it->first <= current_time) {                // Current message should be sent already.
            client.write_buffer += it->second;
            it = client.retarded_responses.erase(it);
        } else {                                        // Not ready to be sent yet.
            ++it;
        }
    }
}

// Calculate the appropriate poll timeout according retarded messages,
// currently waiting to be sent. Returns the number of messages waiting to be sent.
static std::size_t calculate_timeout(int64_t& timeout, detail::Clock const& clock,
                              std::map<int, Client> const& client_map) {
    timeout = -1;
    int64_t current_time = clock.get_time();
    std::size_t retarded_count = 0;

    // Iterate over retarded messages for all clients.
    for (const auto& [_, client] : client_map) {
        for (const auto& [time, _] : client.retarded_responses) {
            ++retarded_count;
            if (timeout < 0 || time - current_time < timeout) {     // Found a smaller necessary timeout.
                timeout = time - current_time;
            }
        }
    }

    return retarded_count;
}

// End the game, send SCORING messages to all participants, 
// disconnect all clients and reset the game state.
static void end_game(Config const& config, std::vector<pollfd>& fds,
                     std::map<int, Client>& client_map,
                     uint32_t& received_puts) {

    // Calculate the score of each client.
    for (std::size_t i = 1; i < fds.size(); ++i) {
        if (fds[i].fd >= 0) {
            for (std::size_t j = 0; j <= config.k; ++j) {
                double val = detail::poly_val(client_map[fds[i].fd].coeffs, j);
                double dev = val - client_map[fds[i].fd].state[j];
                client_map[fds[i].fd].score += (dev * dev);
            }
        }
    }

    // Get the score of each client along its name and sort the result.
    std::vector<std::pair<std::string, double>> scoring;
    for (const auto& [fd, client] : client_map) {
        scoring.emplace_back(client.player_id, client.score);
    }
    std::sort(scoring.begin(), scoring.end());

    std::cout << "Game end, scoring:";
    for (const auto& [player, score] : scoring) {
        std::cout << " " << player << " "
                    << std::fixed << std::setprecision(7) << score;
    }
    std::cout <<".\n";

    // Send SCORING message to all clients.
    std::string scoring_message = build_scoring(scoring);
    for (std::size_t i = 1; i < fds.size(); ++i) {
        if (fds[i].fd < 0) {
            continue;
        }

        send(fds[i].fd, scoring_message.data(), scoring_message.size(), 0);
        disconnect_client(fds, i, client_map, received_puts);
    }
}

// Handle a HELLO message, return false if message is invalid.
static bool handle_hello(std::string const& message, Client& client,
                         bool& valid, std::ifstream& file) {
    if (!validate_hello(message) || client.received_hello) {
        invalidate_message(client, message, valid);
        return false;
    } else {
        client.player_id = parse_hello(message);
        client.received_hello = true;
        client.retardation = detail::count_lowercase(client.player_id) * 1000;

        std::cout << client.ip_str << ":" << client.port << " is now known as " 
                  << client.player_id << ".\n";
        
        // Get the COEFF message from given file.
        std::string coeff_message;
        std::getline(file, coeff_message);
        coeff_message += '\n';
        client.write_buffer += coeff_message;

        client.coeffs = parse_coeff(coeff_message.substr(0, coeff_message.size() - 2));
        std::cout << client.player_id << " get coefficients:";
        for (const auto& coeff : client.coeffs) {
            std::cout  << " " << std::fixed << std::setprecision(7) << coeff;
        }
        std::cout << ".\n";
        return true;
    }
}

// Handle a PUT message, return false if message is invalid or unexpected.
static bool handle_put(std::string const& message, Client& client,
                       bool& valid, detail::Clock const& clock,
                       Config const& config, uint32_t& received_puts) {
    bool bad_put = false;

    // PENALTY necessary.
    auto it = client.retarded_responses.lower_bound(clock.get_time());
    if (!client.received_hello || 
            (it == client.retarded_responses.begin() && !client.retarded_responses.empty())) {
        invalidate_message(client, message, valid);
        std::string penalty_message = "PENALTY "
                                    + message.substr(PUT_SIZE)
                                    + "\r\n";

        client.retarded_responses.insert(
            {clock.get_time(), penalty_message}
        );

        client.score += 20;
    }

    // BAD_PUT necessary: regex match failed.
    if (!validate_put(message)) {
        invalidate_message(client, message, valid);

        std::string bad_put_message = "BAD_PUT "
                                    + message.substr(PUT_SIZE)
                                    + "\r\n";
        
        client.retarded_responses.insert(
            {clock.get_time() + 1000, bad_put_message}
        );
        
        client.score += 10;
        bad_put = true;
    }

    // No BAD_PUT detected yet.
    if (!bad_put) {
        auto [point, value] = parse_put(message);

        // BAD_PUT necessary: out of bounds values.
        if (point < 0 || point > config.k || value < -5.0 || value > 5.0) {
            invalidate_message(client, message, valid);
            std::string bad_put_message = "BAD_PUT "
                                        + message.substr(PUT_SIZE)
                                        + "\r\n";
            
            client.retarded_responses.insert(
                {clock.get_time() + 1000, bad_put_message}
            );
            
            client.score += 10;
        } else if (valid) {     // Correct PUT.
            ++client.sent_puts;
            ++received_puts;
            client.state[point] += value;

            std::cout << client.player_id << " puts " << std::fixed << std::setprecision(7)
                    << value << " in " << point << ", current state:";
            for (const auto& val : client.state) {
                std::cout << " " << std::fixed << std::setprecision(7) << val;
            }
            std::cout << ".\n";

            client.retarded_responses.insert(
                {clock.get_time() + client.retardation,
                build_state(client.state)}
            );

            return true;
        }
    }

    return false;
}

// Parse received messages in a loop.
static bool parse_loop(std::vector<pollfd>& fds, std::size_t i,
                       Config const& config, std::ifstream& file,
                       std::map<int, Client>& client_map,
                       uint32_t& received_puts, detail::Clock const& clock) {

    Client& client = client_map[fds[i].fd];
    while (received_puts < config.m) {
        auto line = extract_line(client.read_buffer);
        if (!line.has_value()) {
            break;
        }

        std::string message = line.value();
        MessageType type = get_message_type(message);
        bool valid = true;

        switch (type) {
            case MessageType::HELLO:
                handle_hello(message, client, valid, file);

                break;
            case MessageType::PUT:
                handle_put(message, client, valid, clock, config, received_puts);

                break;
            default:
                invalidate_message(client, message, valid);
        }

        // If the first message from a client is invalid, disconnect.
        if (!valid && !client.received_hello) {
            err("Invalid first message from [", client.ip_str, "]:", client.port, ", ",
                client.player_id, ": ", message, ", closing connection.");
            disconnect_client(fds, i, client_map, received_puts);
            return false;
        }
    }

    return true;
}

// Main server control loop.
static void run_server(Config const& config, std::vector<pollfd> fds,
                       std::map<int, Client> client_map, detail::Clock const& clock,
                       uint32_t& received_puts, std::ifstream& file) {

    // Block on poll, skip non fatal errors.
    int64_t timeout = -1;
    while (running) {
        if (!wait_on_poll(fds, timeout)) {
            continue;
        }

        // If the game still lasts and a client is waiting, accept his connection.
        if (received_puts < config.m && (fds[0].revents & POLLIN)) {
            accept_connection(config, fds, client_map, clock);
        }

        // Iterate over clients.
        for (std::size_t i = 1; i < fds.size(); ++i) {
            // Empty socket, skip iteration.
            if (fds[i].fd < 0) {
                continue;
            }

            // Check for poll errors, they will result in a disconnect, so skip iteration.
            if (!check_poll_error(fds, i, client_map, received_puts)) {
                continue;
            }
            
            // If the game still lasts, read messages from client.
            // Errors will result in a disconnect, so skip iteration.
            if (received_puts < config.m && (fds[i].revents & POLLIN)) {
                if (receive_messages(fds, i, client_map, received_puts)) {
                    continue;
                }
            }

            // Parse received messages. If the first message is invalid, it will result
            // in a disconnect, so skip iteration.
            if (!parse_loop(fds, i, config, file, client_map, received_puts, clock)) {
                continue;
            }

            // Process retarded responses and put ready ones in write buffer.
            if (fds[i].fd > 0) {
                process_retarded_messages(clock, client_map[fds[i].fd]);
            }

            // If no HELLO message arrives in a given interval after connection, disconnect and skip iteration.
            if (clock.get_time() > client_map[fds[i].fd].connect_time + DISCONNECT_TIMEOUT &&
                !client_map[fds[i].fd].received_hello && fds[i].fd > 0) {
                
                err("Failed to receive 'HELLO' from [", client_map[fds[i].fd].ip_str, "]:", 
                       client_map[fds[i].fd].port, ", closing connection.");
                disconnect_client(fds, i, client_map, received_puts);
                continue;
            }
            
            // Send messages.
            if (!client_map[fds[i].fd].write_buffer.empty() && (fds[i].revents & POLLOUT)) {
                if (!send_messages(fds, i, client_map, received_puts)) {
                    continue;
                }
            }
        }

        // Calculate current poll timeout and count awaiting responses.
        std::size_t retarded_count = calculate_timeout(timeout, clock, client_map);

        // End the game if enough PUTs went through and no messages await.
        if (received_puts == config.m && retarded_count == 0) {
            end_game(config, fds, client_map, received_puts);
        }
    }
}

int main(int argc, char *argv[]) {
    // Parse the command line arguments and initiate internal clock.
    Config config = parse_args(argc, argv);
    detail::Clock clock{};

    // Install a handler for SIGINT and disable cout buffering.
    detail::install_signal_handler(SIGINT, catch_int, SA_RESTART);
    std::cout.setf(std::ios::unitbuf);

    // Open the file containing COEFF messages.
    std::ifstream file(config.file_name);
    if (!file.is_open()) {
        fatal("Failed to open file.");
    }

    // Create an appropriate socket and extract the bind port if OS assigned.
    int sock = detail::create_and_bind_socket(config.port);
    if (config.port == 0) {
        config.port = detail::get_real_port(sock);
    }

    // Listen on the connection socket.
    if (listen(sock, CONNECT_QUEUE_SIZE) < 0) {
        fatal("Failed to listen on socket.");
    }

    std::cout << "Listening on port " << config.port << "\n";

    std::map<int, Client> client_map;
    std::vector<pollfd> fds{1};
    uint32_t received_puts = 0;

    // Setup the poll structure.
    fds[0].fd = sock;
    fds[0].events = POLLIN;

    // Main server logic.
    run_server(config, fds, client_map, clock, received_puts, file);

    // Close open connections if any persist.
    for (std::size_t i = 1; i < fds.size(); ++i) {
        if (fds[i].fd != -1) {
            disconnect_client(fds, i, client_map, received_puts);
        }
    }

    file.close();
    close(sock);
}