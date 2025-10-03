#include <string>
#include <getopt.h>
#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>
#include <regex>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <signal.h>

#include "protocol.h"
#include "detail.h"
#include "err.h"

// Useful constraints.
static constexpr std::size_t BUFFER_SIZE = 1024;

// Simple enum on operating modes.
enum class Mode {
    AUTOMATIC,
    MANUAL
};

// Simple struct to hold console parameters.
struct Config {
    std::string player_id{""};      // Client player id.
    sockaddr_storage server{};      // Server address.
    Mode mode{Mode::MANUAL};        // Client mode. 
    std::string ip_str{};           // Server ip address.
    uint16_t port{};                // Server port.
};

// Simple struct to hold current state of the program.
struct ClientState {
    std::string write_buffer;       // Write buffer.
    std::string read_buffer;        // Read buffer.
    std::string coeff_buffer;       // Coeff buffer (for delaying MANUAL PUTs).
    std::string stdin_buffer;       // STDIN buffer (for MANUAL).
    std::vector<double> coeffs;     // Client polynomial coefficients.
    std::vector<double> state;      // Current polynomial approximation state.
    bool received_coeff = false;    // Flag whether server sent COEFF message. (for MANUAL)
    bool received_message = false;  // Flage whether server sent any message.
    bool put_eligible = false;      // Flag whether client is eligible to send a PUT (for AUTOMATIC).
    int16_t sent = 0;               // Number of sent PUTs (for AUTOMATIC). 
    int16_t K = 1000;               // Available PUT points (initial value is arbitrary if its positive).
};

// Flag to determine whether the program should continue running.
// Changed by catching SIGINT or when server disconnects.
static bool running = true;

// Cancel the while loop after receiving a signal.
static void catch_int([[maybe_unused]] int) {
    running = false;
}

// Print the usage message and exit the program.
[[noreturn]] static void print_usage(char* progname) {
    fatal("Usage: ", progname, " -u <player_id> -s <server> -p <port> [-4] [-6] [-a]");
}

// Parse the program parameters.
static Config parse_args(int argc, char* argv[]) {
    Config config{};

    std::string server = "";
    uint16_t port = 0;

    bool given_u = false, given_s = false, given_p = false, given_4 = false, given_6 = false, given_a = false;

    opterr = 0;
    int opt;
    while ((opt = getopt(argc, argv, "u:s:p:46a")) != -1) {
        switch (opt) {
            case 'u':
                if (given_u) {
                    print_usage(argv[0]);
                }

                if (!detail::is_valid_player_id(std::string(optarg))) {
                    fatal("Invalid player ID:", optarg);
                }

                config.player_id = std::string(optarg);
                given_u = true;
                break;
            case 's':
                if (given_s) {
                    print_usage(argv[0]);
                }

                server = std::string(optarg);
                given_s = true;
                break;
            case 'p':
                if (given_p) {
                    print_usage(argv[0]);
                }

                port = detail::read_port(optarg);
                given_p = true;
                break;

            case '4':
                if (given_4) {
                    print_usage(argv[0]);
                }

                given_4 = true;
                break;

            case '6':
                if (given_6) {
                    print_usage(argv[0]);
                }
                
                given_6 = true;
                break;
            
            case 'a':
                if (given_a) {
                    print_usage(argv[0]);
                }
                
                given_a = true;
                break;
            default:
                print_usage(argv[0]);
        }
    }

    // Unexpected arguments.
    if (optind < argc) {
        fatal("Unexpected positional argument: ", argv[optind]);
    }

    if (!(given_u && given_s && given_p)) {
        fatal("Player ID, server address and port must be specified.");
    }

    // Set the appropriate IP version.
    int ai_family = AF_UNSPEC;
    if (given_4 && given_6) {
        ai_family = AF_UNSPEC;
    } else if (given_4) {
        ai_family = AF_INET;
    } else if (given_6) {
        ai_family = AF_INET6;
    }

    config.server = detail::get_server_address(server, port, ai_family);
    config.mode = given_a ? Mode::AUTOMATIC : Mode::MANUAL;

    return config;
}

// Parse stdin input.
static std::optional<std::pair<int16_t, double>> parse_input(std::string const& input) {
    static const std::regex pattern(R"(^\d+ -?\d+(\.\d{0,7})?$)");
    std::istringstream iss(input);
    int16_t point;
    double value;

    if (!std::regex_match(input, pattern)) {
        return std::nullopt;
    }

    iss >> point >> value;
    return std::make_pair(point, value);
}

// Poll on sockets, return true if poll goes through.
// Exit fatally if poll fails, return false if it was interrupted.
// Exit gracefully if poll detects a socket error.
static bool wait_on_poll(std::vector<pollfd>& fds) {
    int poll_result = poll(fds.data(), fds.size(), -1);
    if (poll_result < 0) {
        if (errno == EINTR) {
            return false;
        }
        fatal("Poll failed.");
    }

    if (fds[0].revents & (POLLHUP | POLLERR | POLLNVAL)) {
        if (running) {
            err("Server disconnected or error with poll occurred.");
            running = false;
        }

        return false;
    }

    return true;
}

// Write to socket.
// Closes the socket if errors occur.
// Returns false if the write results in a disconnect.
static bool send_messages(std::vector<pollfd> const& fds, std::string& write_buffer) {
    if (!write_buffer.empty() && (fds[0].revents & POLLOUT)) {
        ssize_t sent = send(fds[0].fd, write_buffer.data(), write_buffer.size(), 0);
        if (sent < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                sent = 0;
            } else {
                err("Server disconnected or failed to send data to server.");
                running = false;
                return false;
            }
        }
        write_buffer.erase(0, sent);
    }

    return true;
}

// Read from a socket.
// Closes the socket if errors occur.
// Returns false if the read results in a disconnect.
static bool receive_messages(std::vector<pollfd> const& fds, char* buffer, std::string& read_buffer) {
    ssize_t received = recv(fds[0].fd, buffer, sizeof(buffer), 0);
    if (received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return true;
        } else {
            err("Server disconnected or failed to receive data from server.");
            running = false;
            return false;
        }
    } else if (received == 0) {
        if (running) {
            err("unexpected server disconnect");
            running = false;
        }

        return false;
    } else {
        read_buffer.append(buffer, received);
        return true;
    }
}

// Print error message and change a referenced flag.
// Used basically as a C macro.
static void invalidate_message(std::string const& message, Config const& config, bool& valid) {
    msg_error(config.ip_str, config.port, message, config.player_id);
    valid = false;
}

// Handle a COEFF message, return false if message is invalid.
static bool handle_coeff(std::string const& message, Config const& config,
                         bool& received_coeff, bool& valid,
                         std::vector<double>& coeffs) {
    if (!validate_coeff(message) || received_coeff) {
        invalidate_message(message, config, valid);
        return false;
    } else {
        coeffs = parse_coeff(message);
        std::cout << "Received coefficients:";
        for (auto const& coeff : coeffs) {
            std::cout << " " << std::fixed << std::setprecision(7) << coeff;
        }
        std::cout << "\n";
        
        received_coeff = true;
        return true;
    }
}

// Handle a SCORING message, return false if message is invalid.
static bool handle_scoring(std::string const& message, Config const& config,
                           bool& received_coeff, bool& valid) {
    if (!validate_scoring(message) || !received_coeff) {
    invalidate_message(message, config, valid);
        return false;
    } else {
        auto scoring = parse_scoring(message);
        std::cout << "Game end, scoring:";
        for (const auto& [player_id, score] : scoring) {
            std::cout << " " << player_id << " " << std::fixed << std::setprecision(7) << score;
        }
        std::cout << "\n";

        running = false;
        return true;
    }
}

// Handle a BAD_PUT message, return false if message is invalid.
static bool handle_bad_put(std::string const& message, Config const& config,
                           bool& received_coeff, bool& valid) {
    if (!validate_bad_put(message) || !received_coeff) {
        invalidate_message(message, config, valid);
        return false;
    } else {
        auto [point, value] = parse_bad_put(message);
        std::cout << "Received bad put at point " << static_cast<int>(point) << " with value " 
                    << std::fixed << std::setprecision(7) << value << "\n";

        return true;
    }
}

// Handle a PENALTY message, return false if message is invalid.
static bool handle_penalty(std::string const& message, Config const& config,
                           bool& received_coeff, bool& valid) {
    if (!validate_penalty(message) || !received_coeff) {
        invalidate_message(message, config, valid);
        return false;
    } else {
        auto [point, value] = parse_penalty(message);
        std::cout << "Received penalty at point " << static_cast<int>(point) << " with value " 
                    << std::fixed << std::setprecision(7) << value << "\n";
        return true;
    }
}

// Handle a STATE message, return false if message is invalid.
static bool handle_state(std::string const& message, Config const& config,
                         bool& received_coeff, bool& valid,
                         std::vector<double>& state) {
    if (!validate_state(message) || !received_coeff) {
        invalidate_message(message, config, valid);
        return false;
    } else {
        state = parse_state(message);
        std::cout << "Received state:";
        for (const auto& val : state) {
            std::cout << " " << std::fixed << std::setprecision(7) << val;
        }
        std::cout << "\n";
        return true;
    }
}

// Deduce the next PUT to be sent and add it to buffer.
// Used in the AUTOMATIC mode.
static void automatic_put(ClientState& cs) {
    std::cout << "Putting " << detail::poly_val(cs.coeffs, cs.sent)
              << " in " << cs.sent << ".\n";
    cs.write_buffer += build_put(cs.sent, detail::poly_val(cs.coeffs, cs.sent));
    ++cs.sent;
    cs.put_eligible = false;
}

// Handle a message depending on its type and operating mode.
// Return false if message was invalid.
static bool handle_message(Mode mode, MessageType type, Config const& config,
                           ClientState& cs, std::string const& message) {
    bool valid = true;
    switch (type) {
        case MessageType::COEFF:
            if (handle_coeff(message, config, cs.received_coeff, valid, cs.coeffs)) {
                // Allow PUT sending, implementation depends on mode.
                if (mode == Mode::AUTOMATIC) {         
                    cs.put_eligible = true;
                } else if (mode == Mode::MANUAL) {
                    cs.write_buffer += cs.coeff_buffer;
                    cs.coeff_buffer.clear();
                }
            }

            break;
        case MessageType::SCORING:
            handle_scoring(message, config, cs.received_coeff, valid);

            break;
        case MessageType::BAD_PUT:
            if (handle_bad_put(message, config, cs.received_coeff, valid)) {
                if (mode == Mode::AUTOMATIC) {
                    cs.put_eligible = true;      // Automatic mode waits for response before PUTs.
                }
            }

            break;
        case MessageType::PENALTY:
            if (handle_penalty(message, config, cs.received_coeff, valid)) {
                if (mode == Mode::AUTOMATIC) {
                    cs.put_eligible = true;     // Automatic mode waits for response before PUTs.
                }
            }

            break;
        case MessageType::STATE:
            if (handle_state(message, config, cs.received_coeff, valid, cs.state)) {
                if (mode == Mode::AUTOMATIC) {
                    cs.put_eligible = true;     // Automatic mode waits for response before PUTs.
                    cs.K = cs.state.size();     // Read the STATE size to send appropriate PUTs.
                }
            }

            break;
        default:
            invalidate_message(message, config, valid);
    }

    return true;
}

// Read from stdin and send an appropriate PUT message.
// Used in the MANUAL mode.
static void read_stdin(ClientState& cs) {
    std::getline(std::cin, cs.stdin_buffer);
    auto input = parse_input(cs.stdin_buffer);
    
    // Stdin input must be correct.
    if (input.has_value()) {
        std::cout << "Putting " << input->second << " in " << input->first << ".\n";
        if (!cs.received_coeff) {
            cs.coeff_buffer += build_put(input->first, input->second);  // Wait for COEFF before adding to write buffer.
        }
        else {
            cs.write_buffer += build_put(input->first, input->second);
        }
    } else {
        err("invalid input line ", cs.stdin_buffer);
    }
}

// Loop over received messages and handle them.
static void parse_loop(Mode mode, Config const& config, ClientState& cs) {
    while (true) {
        auto line = extract_line(cs.read_buffer);
        if (!line.has_value()) {
            break;
        }

        std::string message = line.value();
        MessageType type = get_message_type(message);

        bool valid = handle_message(mode, type, config, cs, message);

        // If the first message is invalid, disconnect fatally.
        if (!valid && !cs.received_message) {
            fatal("Invalid first message.");
        }

        cs.received_message = true;
    }
}

// Main client control loop.
// Behaviour depends on the mode.
static void run_client(Mode mode, Config const& config, int sock) {
    ClientState cs{};
    std::vector<pollfd> fds{};
    char buffer[BUFFER_SIZE];

    cs.write_buffer += build_hello(config.player_id);

    // Prepare pollfd structures according to mode.
    if (mode == Mode::AUTOMATIC) {
        fds.resize(1);
    } else if (mode == Mode::MANUAL) {
        fds.resize(2);
        fds[1].fd = STDIN_FILENO;
        fds[1].events = POLLIN;
    } else {
        fatal("Unrecognized mode");
    }

    // Common pollfd preparation.
    fds[0].fd = sock;
    fds[0].events = POLLIN | POLLOUT;

    while (running) {
        // Block on poll, non-fatal poll error will result in
        // a graceful disconnect, so skip the iteration.
        if (!wait_on_poll(fds)) {
            continue;
        }
        
        // Send prepared messages, non-fatal send error will result in
        // a graceful disconnect, so skip the iteration.
        if (!send_messages(fds, cs.write_buffer)) {
            continue;
        }

        // Parse received messages in a loop.
        parse_loop(mode, config, cs);

        // Disjoint mode logic.
        if (mode == Mode::AUTOMATIC) {
            if (cs.put_eligible && cs.sent <= cs.K) {
                automatic_put(cs);
            }
        } else if (mode == Mode::MANUAL) {
            if (fds[1].revents & POLLIN) {
                read_stdin(cs);
            }
        }
        
        // Receive messages from socket, non-fatal recv error will
        // result in a graceful disconnect, but no need to skip the iteration 
        // as we're already at the end.
        if (fds[0].revents & POLLIN) {
            receive_messages(fds, buffer, cs.read_buffer);
        }
    }
}

int main(int argc, char *argv[]) {
    // Parse the command line arguments.
    Config config = parse_args(argc, argv);

    // Install a handler for SIGINT and disable cout buffering.
    detail::install_signal_handler(SIGINT, catch_int, SA_RESTART);
    std::cout.setf(std::ios::unitbuf);

    // Create a socket.
    int sock = socket(config.server.ss_family, SOCK_STREAM, 0);
    if (sock < 0) {
        fatal("Failed to create socket.");
    }

    // Connect to the server.
    if (connect(sock, reinterpret_cast<sockaddr*>(&config.server), sizeof(config.server)) < 0) {
        if (errno != EINPROGRESS) {
            fatal("Failed to connect to server.");
        }
    }

    std::tie(config.ip_str, config.port) = detail::get_representation(config.server);
    std::cout << "Connected to [" << config.ip_str << "]:" << config.port << "\n";

    // Main client logic loop.
    run_client(config.mode, config, sock);

    close(sock);
}