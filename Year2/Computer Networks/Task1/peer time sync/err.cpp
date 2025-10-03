#include "err.h"

#include <iomanip>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "protocol.h"

// Print an error message related to message processing.
void msg_error(uint8_t const* buffer) {
    if (buffer == nullptr) {
        fatal("Buffer is null");
    }

    std::cerr << "ERROR MSG ";

    // Get the message size based on the type.
    // Default to 10 bytes for HELLO_REPLY, which has variable size.
    std::size_t message_size = get_message_size(static_cast<MESSAGE>(buffer[0]));
    if (static_cast<MESSAGE>(buffer[0]) == MESSAGE::HELLO_REPLY) {
        message_size = 10;
    }

    // Print the bytes.
    for (std::size_t i = 0; i < message_size; ++i) {
        std::cerr << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[i]);
    }

    std::cerr << std::dec << "\n";
}