#include <iostream>
#include <string>
#include <getopt.h>
#include <optional>
#include <cstdint>
#include <cstddef>
#include <signal.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/time.h>
#include <set>
#include <map>
#include <array>

#include "protocol.h"
#include "err.h"
#include "detail.h"

using peer_set = std::set<sockaddr_in, detail::sockaddr_in_cmp>;
using peer_time_map = std::map<sockaddr_in, int64_t, detail::sockaddr_in_cmp>;
using delay_request_map = std::map<sockaddr_in, bool, detail::sockaddr_in_cmp>;

// Simple struct to hold console parameters in sockaddr_in.
struct Config {
    sockaddr_in bind_address;
    std::optional<sockaddr_in> peer_address;
};

// Current context/state of the node.
struct NodeContext {
    int my_socket;                              // Socket id.
    detail::NodeClock natural_clock;            // Natural clock.
    int64_t offset;                             // Current offset (0) if not synced.
    uint8_t synchronized;                       // Sync level of the node we are syncing from.
    Config config;                              // Console parameters.
    peer_set peers;                             // Known peers.
    peer_set pending_peers;                     // Peers awaiting connection confirmation.
    std::optional<sockaddr_in> sync_from;       // Node from which we are synced.
    std::optional<sockaddr_in> syncing_from;    // Node from which we are in the process of syncing.
    std::optional<uint8_t> sync_synchronized;   // Above node's sync level.
    std::array<int64_t, 4> sync_times;          // Times necessary to calculate sync offset. 
    std::optional<int64_t> sync_attempt_from;   // Start time of our current syncing process.
    peer_time_map sync_attempt_to;              // Last sync attempt we sent to our peers.
    std::optional<int64_t> last_sync_time;      // Last sync attempt from the node we are synced from.
    std::optional<int64_t> leader_time;         // Time passed after becoming a leader.
    delay_request_map delay_requests_received;  // Did we receive DELAY_REQUEST from given node
                                                // during this syncing process.
};

constexpr int64_t SYNC_TIMEOUT_MS = 5000;           // Timeout for the syncing process.
constexpr int64_t SYNC_LOSS_TIMEOUT_MS = 20000;     // Timeout for loosing sync after we haven't 
                                                    // received SYNC_START from the node we're synced from.

constexpr int64_t SYNC_SENDING_INTERVAL_MS = 5000;  // Interval for sending SYNC_START.
constexpr int64_t LEADER_TIMEOUT_MS = 2000;         // Waiting period after receiving becoming a leader.

static uint8_t buffer[MAX_PACKET_SIZE];
static bool finished = false;

// Cancel the while loop after receiving a signal.
void catch_int([[maybe_unused]] int) {
    finished = true;
}

// Parse the program parameters.
Config parse_args(int argc, char* argv[]) {
    Config config{};

    std::string bind_address = "";
    uint16_t bind_port = 0;
    std::string peer_address = "";
    uint16_t peer_port = 0;

    bool b_given = false, p_given = false, a_given = false, r_given = false;

    int opt;
    while ((opt = getopt(argc, argv, "b:p:a:r:")) != -1) {
        switch (opt) {
            case 'b':
                if (b_given) {
                    fatal("Usage:",  argv[0], "-b <bind_address> -p <bind_port> -a <peer_address> -r <peer_port>");
                    break;
                }

                bind_address = optarg;
                b_given = true;
                break;
            
            case 'p':
                if (p_given) {
                    fatal("Usage:",  argv[0], "-b <bind_address> -p <bind_port> -a <peer_address> -r <peer_port>");
                    break;
                }
                
                if (std::string(optarg) == "0") {
                    p_given = true;
                    break;
                }

                bind_port = detail::read_port(optarg);
                p_given = true;
                break;

            case 'a':
                if (a_given) {
                    fatal("Usage:",  argv[0], "-b <bind_address> -p <bind_port> -a <peer_address> -r <peer_port>");
                    break;
                }

                peer_address = optarg;
                a_given = true;
                break;

            case 'r':
                if (r_given) {
                    fatal("Usage:",  argv[0], "-b <bind_address> -p <bind_port> -a <peer_address> -r <peer_port>");
                    break;
                }
                
                peer_port = detail::read_port(optarg);
                
                r_given = true;
                break;

            default:
                fatal("Usage:",  argv[0], "-b <bind_address> -p <bind_port> -a <peer_address> -r <peer_port>");
        }
    }

    // Unexpected arguments.
    if (optind < argc) {
        fatal("Unexpected positional argument: ", argv[optind]);
    }

    // Peer address and port must be given together.
    if (a_given && !r_given) {
        fatal("Peer port must be specified if peer address is given.");
    } else if (!a_given && r_given) {
        fatal("Peer address must be specified if peer port is given.");
    }

    // Get peer address.
    if (a_given && r_given) {
        config.peer_address = detail::get_peer_address(peer_address, peer_port);
    } else {
        config.peer_address = std::nullopt;
    }

    config.bind_address = detail::get_bind_address(bind_address, bind_port);

    return config;
}

// Handle GET_TIME message.
// Return current time, with offset.
void get_time_handler(NodeContext const& context, sockaddr_in const* msg_address) {
    MessageHeader response{};
    response.type = MESSAGE::TIME;
    response.timestamp = context.natural_clock.get_time() - context.offset;
    response.synchronized = context.synchronized;

    serialize_header(response, buffer);

    ssize_t sent = sendto(context.my_socket, buffer, TIME_SIZE, 0,
                          reinterpret_cast<sockaddr const*>(msg_address),
                          sizeof(*msg_address));

    if (sent < 0) {
        msg_error(buffer);
    }
}

// Handle HELLO message.
// Send HELLO_REPLY with known peers.
void hello_handler(NodeContext& context, sockaddr_in const* msg_address) {
    MessageHeader response{};
    response.type = MESSAGE::HELLO_REPLY;

    if (context.peers.contains(*msg_address)) {
        msg_error(buffer);
        return;
    }

    // Message to be sent will be to big.
    if (HELLO_REPLY_SIZE + context.peers.size() * 7 > MAX_PACKET_SIZE) {
        msg_error(buffer);
        return;
    }

    response.count = context.peers.size();
    serialize_header(response, buffer);

    uint8_t* ptr = buffer + HELLO_REPLY_SIZE;

    // Iterate through known peers and save them to the buffer.
    for (auto const& peer : context.peers) {
        uint16_t peer_address_length = htons(4);
        std::memcpy(ptr, &peer_address_length, sizeof(peer_address_length));
        ptr += sizeof(peer_address_length);
        std::memcpy(ptr, &peer.sin_addr.s_addr, sizeof(peer.sin_addr.s_addr));
        ptr += sizeof(peer.sin_addr.s_addr);
        std::memcpy(ptr, &peer.sin_port, sizeof(peer.sin_port));
    }

    ssize_t sent = sendto(context.my_socket, buffer, HELLO_REPLY_SIZE + context.peers.size() * 8, 0,
                          reinterpret_cast<sockaddr const*>(msg_address),
                          sizeof(*msg_address));
    
    if (sent < 0) {
        msg_error(buffer);
        return;
    }

    context.peers.insert(*msg_address);
}

// Handle HELLO_REPLY message.
// Save peers and send them CONNECT.
void hello_reply_handler(NodeContext& context, sockaddr_in const* msg_address, ssize_t len) {
    // To little received.
    if ((std::size_t)len < HELLO_REPLY_SIZE) {
        msg_error(buffer);
        return;
    }

    // Ignore if received not from our original peer.
    if (!context.config.peer_address.has_value() ||
        !detail::is_same_sockaddr_in(*msg_address, context.config.peer_address.value())) {
        msg_error(buffer);
        return;
    }

    MessageHeader header = deserialize_header(buffer);

    // Incorrect message due to errors in size.
    if ((std::size_t)len != HELLO_REPLY_SIZE + 7 * header.count.value()) {
        msg_error(buffer);
        return;
    }

    // Iterate through received peers and add them to pending peers.
    uint8_t* ptr = buffer + HELLO_REPLY_SIZE;
    for (uint16_t i = 0; i < header.count.value(); ++i) {
        // Copy the address length.
        uint8_t peer_address_length;
        std::memcpy(&peer_address_length, ptr, sizeof(peer_address_length));
        ptr += sizeof(peer_address_length);

        // We work only in IPv4.
        if (peer_address_length != 4) {
            msg_error(buffer);
            return;
        }

        // Copy the peer address.
        uint32_t peer_address;
        std::memcpy(&peer_address, ptr, sizeof(peer_address));
        ptr += sizeof(peer_address);

        // Cop the peer port.
        uint16_t peer_port;
        std::memcpy(&peer_port, ptr, sizeof(peer_port));

        // Invalid port.
        if (peer_port == 0)  {
            context.pending_peers.clear();
            msg_error(buffer);
            return;
        }

        sockaddr_in peer{};
        peer.sin_family = AF_INET;
        peer.sin_addr.s_addr = peer_address;
        peer.sin_port = peer_port;

        context.pending_peers.insert(peer);

        // If peer is us or sender, ignore message.
        if (detail::is_me(&context.config.bind_address, &peer) ||
            detail::is_same_sockaddr_in(peer, context.config.peer_address.value())) {
            context.pending_peers.clear();
            msg_error(buffer);
            return;
        }
    }

    context.peers.insert(*msg_address);

    // Iterate through received peers and send CONNECT.
    for (auto const& peer : context.pending_peers) {
        MessageHeader response{};
        response.type = MESSAGE::CONNECT;

        serialize_header(response, buffer);

        ssize_t sent = sendto(context.my_socket, buffer, CONNECT_SIZE, 0,
                              reinterpret_cast<sockaddr const*>(&peer), sizeof(peer));
        
        if (sent < 0) {
            msg_error(buffer);
            continue;
        }
    }
}

// Handle CONNECT message.
// Send ACK_CONNECT and add to known peers.
void connect_handler(NodeContext& context, sockaddr_in const* msg_address) {
    if (context.pending_peers.contains(*msg_address)) {
        msg_error(buffer);
        return;
    }

    context.peers.insert(*msg_address);
    context.pending_peers.erase(*msg_address);
    MessageHeader response{};
    response.type = MESSAGE::ACK_CONNECT;
    serialize_header(response, buffer);

    ssize_t sent = sendto(context.my_socket, buffer, ACK_CONNECT_SIZE, 0,
                          reinterpret_cast<sockaddr const*>(msg_address),
                          sizeof(*msg_address));

    if (sent < 0) {
        msg_error(buffer);
    }
}

// Handle ACK_CONNECT message.
// Add to known peers.
void ack_connect_handler(NodeContext& context, sockaddr_in const* msg_address) {
    if (context.pending_peers.contains(*msg_address)) {
        context.pending_peers.erase(*msg_address);
        context.peers.insert(*msg_address);
    } else {
        msg_error(buffer);
    }
}

// Handle SYNC_START message.
// Send DELAY_REQUEST if valid.
void sync_start_handler(NodeContext& context, sockaddr_in const* msg_address) {
    int64_t sync_time1 = context.natural_clock.get_time();
    MessageHeader msg = deserialize_header(buffer);
    int64_t sync_time0 = msg.timestamp.value();

    // Negative time.
    if (sync_time0 < 0) {
        msg_error(buffer);
        return;
    }

    // Unknown peer.
    if (!context.peers.contains(*msg_address)) {
        msg_error(buffer);
        return;
    }

    // Peer not eligible to sync from.
    if (msg.synchronized.value() >= 254) {
        msg_error(buffer);
        return;
    }

    // Synced with sender, his sync level is to high.
    if (msg.synchronized.value() >= context.synchronized &&
        (context.sync_from.has_value() &&
        detail::is_same_sockaddr_in(*msg_address, context.sync_from.value()))) {
        context.sync_from = std::nullopt;
        context.synchronized = 255;
        context.sync_times.fill(0);
        context.sync_synchronized = std::nullopt;
        context.syncing_from = std::nullopt;
        context.sync_attempt_from = std::nullopt;
        context.offset = 0;
        msg_error(buffer);
        return;
    }

    // Not synced with sender, his sync level is to high.
    if (msg.synchronized.value() + 1 >= context.synchronized &&
        (!context.sync_from.has_value() ||
        !detail::is_same_sockaddr_in(*msg_address, context.sync_from.value()))) {
        msg_error(buffer);
        return;
    }

    MessageHeader response{};
    response.type = MESSAGE::DELAY_REQUEST;

    serialize_header(response, buffer);

    ssize_t sent = sendto(context.my_socket, buffer, DELAY_REQUEST_SIZE, 0,
                          reinterpret_cast<sockaddr const*>(msg_address),
                          sizeof(*msg_address));
    
    if (sent < 0) {
        msg_error(buffer);
        return;
    }

    // Saves syncing progress.
    context.sync_attempt_from = sync_time1;
    context.sync_times[2] = context.natural_clock.get_time();
    context.sync_times[1] = sync_time1;
    context.sync_times[0] = sync_time0;
    context.syncing_from = *msg_address;
    context.sync_synchronized = msg.synchronized.value();
}

// Handle DELAY_REQUEST message.
// Validate and send DELAY_RESPONSE.
void delay_request_handler(NodeContext& context, sockaddr_in const* msg_address) {
    // Sender did not receive SYNC_START from us.
    if (!context.sync_attempt_to.contains(*msg_address)) {
        msg_error(buffer);
        return;
    }

    // Syncing timed out.
    if (context.sync_attempt_to[*msg_address] + SYNC_TIMEOUT_MS < context.natural_clock.get_time()) {
        msg_error(buffer);
        return;
    }

    // Already received DELAY_REQUEST from this node during this sync process.
    if (context.delay_requests_received[*msg_address]) {
        msg_error(buffer);
        return;
    }

    context.delay_requests_received[*msg_address] = true;

    MessageHeader response{};
    response.type = MESSAGE::DELAY_RESPONSE;
    response.synchronized = context.synchronized;
    response.timestamp = context.natural_clock.get_time();

    serialize_header(response, buffer);
    ssize_t sent = sendto(context.my_socket, buffer, DELAY_RESPONSE_SIZE, 0,
                          reinterpret_cast<sockaddr const*>(msg_address),
                          sizeof(*msg_address));

    if (sent < 0) {
        msg_error(buffer);
    }
}

// Handle DELAY_RESPONSE message.
// Validate and sync from sender.
void delay_response_handler(NodeContext& context, sockaddr_in const* msg_address) {
    MessageHeader msg = deserialize_header(buffer);

    // Not syncing from sender.
    if (!context.syncing_from.has_value() ||
        !detail::is_same_sockaddr_in(*msg_address, context.syncing_from.value())) {
        msg_error(buffer);
        return;
    }

    // Sender's sync level changed.
    if (!msg.synchronized.has_value() || msg.synchronized.value() != context.sync_synchronized.value()) {
        msg_error(buffer);
        return;
    }

    // Syncing timed out.
    if (context.sync_attempt_from.has_value() &&
        context.sync_attempt_from.value() + SYNC_TIMEOUT_MS < context.natural_clock.get_time()) {
        context.syncing_from = std::nullopt;
        context.sync_attempt_from = std::nullopt;
        msg_error(buffer);
        return;
    }

    // Negative time.
    if (msg.timestamp.value() < 0) {
        context.syncing_from = std::nullopt;
        context.sync_attempt_from = std::nullopt;
        context.sync_times.fill(0);
    }

    context.sync_times[3] = msg.timestamp.value();

    // Calculate offset.
    context.offset =  ((context.sync_times[1] - context.sync_times[0] +
                      context.sync_times[2] - context.sync_times[3]) / 2);
    
    // We are now synced from sender.
    context.sync_from = *msg_address;
    context.synchronized = msg.synchronized.value() + 1;
    context.syncing_from = std::nullopt;
    context.sync_synchronized = std::nullopt;
    context.sync_times.fill(0);
    context.last_sync_time = context.natural_clock.get_time();
}

// Handle LEADER message.
void leader_handler(NodeContext& context) {
    MessageHeader msg = deserialize_header(buffer);

    // We are a leader.
    if (context.synchronized == 0) {
        // Wrong synchronized level.
        if (msg.synchronized.value() != 255) {;
            msg_error(buffer);
            return;
        }

        // We are now not synchronized with anyone and not a leader.
        context.synchronized = 255;
        context.sync_from = std::nullopt;
        context.syncing_from = std::nullopt;
        context.sync_synchronized = std::nullopt;
        context.sync_times.fill(0);
        context.sync_attempt_from = std::nullopt;
        context.offset = 0;
        context.last_sync_time = std::nullopt;
    } else {    // We are not a leader.
        // Wrong synchronized value.
        if (msg.synchronized.value() != 0) {
            msg_error(buffer);
            return;
        }

        // We are now a leader.
        context.synchronized = 0;
        context.sync_from = std::nullopt;
        context.syncing_from = std::nullopt;
        context.sync_synchronized = std::nullopt;
        context.sync_times.fill(0);
        context.sync_attempt_from = std::nullopt;
        context.offset = 0;
        context.last_sync_time = std::nullopt;
        context.leader_time = context.natural_clock.get_time();
    }
}

int main(int argc, char* argv[]) {
    NodeContext context{};
    context.natural_clock = detail::NodeClock{};
    context.synchronized = 255;
    context.offset = 0;
    sockaddr_in msg_address{};
    socklen_t msg_address_len = sizeof(msg_address);
    Config config = parse_args(argc, argv);
    int64_t last_sent_sync = 0;

    context.config = config;

    // Open a socket.
    context.my_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (context.my_socket < 0) {
        syserr("socket");
        std::exit(EXIT_FAILURE);
    }

    // Bind the socket.
    if (bind(context.my_socket,
             reinterpret_cast<sockaddr*>(&context.config.bind_address),
             sizeof(context.config.bind_address)) < 0) {
        syserr("bind");
        std::exit(EXIT_FAILURE);
    }

    // If given port was 0, retrieve the port number assigned by the kernel.
    if(context.config.bind_address.sin_port == 0) {
        socklen_t len = sizeof(context.config.bind_address);
        ssize_t result = getsockname(context.my_socket,
                                     reinterpret_cast<sockaddr*>(&context.config.bind_address),
                                     &len);
        
        if (result < 0) {
            syserr("getsockname");
            std::exit(EXIT_FAILURE);
        }
    }
    
    // Prepare receive timeout.
    timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    // Set receive timeout.
    if (setsockopt(context.my_socket, SOL_SOCKET, SO_RCVTIMEO,
                   &timeout, sizeof(timeout)) < 0) {
        syserr("setsockopt");
        std::exit(EXIT_FAILURE);
    }

    // Override SIGINT.
    detail::install_signal_handler(SIGINT, catch_int, SA_RESTART);

    // Send HELLO if necessary.
    if (config.peer_address.has_value()) {
        MessageHeader hello{};
        hello.type = MESSAGE::HELLO;
        serialize_header(hello, buffer);

        ssize_t sent = sendto(context.my_socket, buffer, HELLO_SIZE, 0,
                              reinterpret_cast<sockaddr const*>(&config.peer_address.value()),
                              sizeof(config.peer_address.value()));  

        if (sent < 0) {
            msg_error(buffer);
        }
    }

    while (!finished) {
        int64_t now = context.natural_clock.get_time();

        bool enough_time_passed = now - last_sent_sync > SYNC_SENDING_INTERVAL_MS;
        bool below_sync_limit = context.synchronized < 254;
        bool leader_delay_passed = true;
        
        if (context.synchronized == 0) {
            leader_delay_passed = context.leader_time.has_value() &&
                                  (now - context.leader_time.value() > LEADER_TIMEOUT_MS);
        }
        
        bool should_sync = enough_time_passed && below_sync_limit && leader_delay_passed;

        // Sending SYNC_START to peers.
        if (should_sync) {
            for (auto const& peer : context.peers) {
                MessageHeader msg{};
                msg.type = MESSAGE::SYNC_START;
                msg.synchronized = context.synchronized;
                msg.timestamp = context.natural_clock.get_time();

                serialize_header(msg, buffer);

                ssize_t sent = sendto(context.my_socket, buffer, SYNC_START_SIZE, 0,
                                      reinterpret_cast<sockaddr const*>(&peer),
                                      sizeof(peer));
                
                if (sent < 0) {
                    msg_error(buffer);
                } else {
                    context.sync_attempt_to[peer] = context.natural_clock.get_time();
                    context.delay_requests_received[peer] = false;
                }
            }

            last_sent_sync = context.natural_clock.get_time();
        }

        // Lose sync after timeout.
        if (context.synchronized != 255 &&
            context.sync_from.has_value() &&
            context.last_sync_time.has_value() &&
            now - context.last_sync_time.value() > SYNC_LOSS_TIMEOUT_MS) {
            context.synchronized = 255;
            context.sync_from = std::nullopt;
            context.last_sync_time = std::nullopt;
        }

        // Terminate syncing process after timeout.
        if (context.syncing_from.has_value() &&
            context.sync_attempt_from.has_value() &&
            now - context.sync_attempt_from.value() > SYNC_TIMEOUT_MS) {
            context.syncing_from = std::nullopt;
            context.sync_attempt_from = std::nullopt;
            context.sync_times.fill(0);
        }

        ssize_t received = recvfrom(context.my_socket, buffer, MAX_PACKET_SIZE, 0,
                                                     reinterpret_cast<sockaddr*>(&msg_address),
                                                     &msg_address_len);
        
        // Recvfrom error.
        if (received < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                syserr("recvfrom");
                std::exit(EXIT_FAILURE);
            } else {
                continue;
            }
        }

        // Received message comes from ourselves.
        if (detail::is_me(&context.config.bind_address, &msg_address)) {
            msg_error(buffer);
            continue;
        }

        MESSAGE msg_type = static_cast<MESSAGE>(buffer[0]);
        
        // Check whether the message size is valid.
        // As hello reply is of variable size, the size checking will be done in the handler.
        if(msg_type != MESSAGE::HELLO_REPLY) {
            if ((std::size_t)received != get_message_size(msg_type)) {
                msg_error(buffer);
                continue;
            }
        }
        
        // React accordingly to message type.
        switch (msg_type) {
            case MESSAGE::GET_TIME:
                get_time_handler(context, &msg_address);
                break;

            case MESSAGE::HELLO:
                hello_handler(context, &msg_address);
                break;
            
            case MESSAGE::HELLO_REPLY:
                hello_reply_handler(context, &msg_address, received);
                break;

            case MESSAGE::CONNECT:
                connect_handler(context, &msg_address);
                break;

            case MESSAGE::ACK_CONNECT:
                ack_connect_handler(context, &msg_address);
                break;

            case MESSAGE::SYNC_START:
                sync_start_handler(context, &msg_address);
                break;

            case MESSAGE::DELAY_REQUEST:
                delay_request_handler(context, &msg_address);
                break;

            case MESSAGE::DELAY_RESPONSE:
                delay_response_handler(context, &msg_address);
                break;

            case MESSAGE::LEADER:
                leader_handler(context);
                break;
                
            default:
                msg_error(buffer);
        }
    }

    close(context.my_socket);
    return 0;
}