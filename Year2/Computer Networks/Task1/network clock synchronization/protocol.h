#ifndef PEER_PROTOCOL_H
#define PEER_PROTOCOL_H

#include <cstdint>
#include <optional>

// General constants.
constexpr std::size_t MAX_ADDRESS_LENGTH = 4;
constexpr std::size_t MAX_PACKET_SIZE = 65507;

// Message sizes.
// Doesn't include HELLO_REPLY's variable size.
constexpr std::size_t HELLO_SIZE = 1;
constexpr std::size_t HELLO_REPLY_SIZE = 3;
constexpr std::size_t CONNECT_SIZE = 1;
constexpr std::size_t ACK_CONNECT_SIZE = 1;
constexpr std::size_t SYNC_START_SIZE = 10;
constexpr std::size_t DELAY_REQUEST_SIZE = 1;
constexpr std::size_t DELAY_RESPONSE_SIZE = 10;
constexpr std::size_t LEADER_SIZE = 2;
constexpr std::size_t GET_TIME_SIZE = 1;
constexpr std::size_t TIME_SIZE = 10;
constexpr std::size_t ERROR_SIZE = 1;

// Enum class for message types.
enum class MESSAGE : uint8_t {
    // Connection.
    HELLO = 1,
    HELLO_REPLY = 2,
    CONNECT = 3,
    ACK_CONNECT = 4,

    // Synchronization.
    SYNC_START = 11,
    DELAY_REQUEST = 12,
    DELAY_RESPONSE = 13,

    // Leader.
    LEADER = 21,

    // Current Time.
    GET_TIME = 31,
    TIME = 32,

    // Error.
    ERROR = 69
};

// Helper function to get the size of a message by type.
// Defaults to 10 bytes if the type is unknown.
constexpr std::size_t get_message_size(MESSAGE type) {
    switch (type) {
        case MESSAGE::HELLO:           return HELLO_SIZE;
        case MESSAGE::HELLO_REPLY:     return HELLO_REPLY_SIZE;
        case MESSAGE::CONNECT:         return CONNECT_SIZE;
        case MESSAGE::ACK_CONNECT:     return ACK_CONNECT_SIZE;
        case MESSAGE::SYNC_START:      return SYNC_START_SIZE;
        case MESSAGE::DELAY_REQUEST:   return DELAY_REQUEST_SIZE;
        case MESSAGE::DELAY_RESPONSE:  return DELAY_RESPONSE_SIZE;
        case MESSAGE::LEADER:          return LEADER_SIZE;
        case MESSAGE::GET_TIME:        return GET_TIME_SIZE;
        case MESSAGE::TIME:            return TIME_SIZE;
        case MESSAGE::ERROR:           return ERROR_SIZE;
        default:                       return 10;
    }
}

// Message header structure.
struct MessageHeader {
    MESSAGE type;
    std::optional<uint16_t> count;
    std::optional<uint8_t> synchronized;
    std::optional<int64_t> timestamp;
};

// Function to serialize the message header into a buffer.
void serialize_header(MessageHeader const& header, uint8_t* buffer);

// Function to deserialize the message header from a buffer.
MessageHeader deserialize_header(uint8_t const* buffer);

#endif // PEER_PROTOCOL_H