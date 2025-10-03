#include "protocol.h"

#include <cstring>
#include <arpa/inet.h>
#include <endian.h>

#include "err.h"

// Function to serialize the message header into a buffer.
void serialize_header(MessageHeader const& header, uint8_t* buffer) {
    if (buffer == nullptr) {
        fatal("Buffer is null");
    }

    std::memset(buffer, 0, MAX_PACKET_SIZE);
    
    // Set the message type.
    uint8_t* ptr = buffer;
    *ptr = static_cast<uint8_t>(header.type);
    ++ptr;

    // Set the 'count' field if present.
    if (header.count.has_value()) {
        uint16_t count = htons(header.count.value());
        std::memcpy(ptr, &count, sizeof(count));
        ptr += sizeof(count);
    }

    // Set the 'synchronized' field if present.
    if (header.synchronized.has_value()) {
        *ptr = header.synchronized.value();
        ++ptr;
    }

    // Set the 'timestamp' field if present.
    if (header.timestamp.has_value()) {
        int64_t timestamp = htobe64(header.timestamp.value());
        std::memcpy(ptr, &timestamp, sizeof(timestamp));
        ptr += sizeof(timestamp);
    }
}

MessageHeader deserialize_header(uint8_t const* buffer) {
    if (buffer == nullptr) {
        fatal("Buffer is null");
    }

    uint8_t  const* ptr = buffer;
    MessageHeader header{};

    // Read the message type.
    header.type = static_cast<MESSAGE>(*ptr);
    ++ptr;

    // Switch on the message type to handle different cases.
    switch (header.type) {
        // Empty message types.
        case MESSAGE::HELLO:
        case MESSAGE::CONNECT:
        case MESSAGE::ACK_CONNECT:
        case MESSAGE::DELAY_REQUEST:
        case MESSAGE::GET_TIME:
            break;

        // Message types wth the 'count' field.
        case MESSAGE::HELLO_REPLY:
            uint16_t count;
            std::memcpy(&count, ptr, sizeof(count));
            header.count = ntohs(count);
            break;

        // Message types with the 'synchronized' and 'timestamp' fields.
        case MESSAGE::SYNC_START:
        case MESSAGE::DELAY_RESPONSE:
        case MESSAGE::TIME:
            header.synchronized = *ptr++;
            int64_t timestamp;
            std::memcpy(&timestamp, ptr, sizeof(timestamp));
            header.timestamp = be64toh(timestamp);

            ptr += sizeof(timestamp);
            break;

        // Message types with the 'synchronized' field.
        case MESSAGE::LEADER:
            header.synchronized = *ptr;
            break;
        
        // Error message type.
        default:
            header.type = MESSAGE::ERROR;
            break;
    }

    return header;
}