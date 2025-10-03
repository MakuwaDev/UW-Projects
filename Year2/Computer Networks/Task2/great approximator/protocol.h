#ifndef APPROX_PROTOCOL_H
#define APPROX_PROTOCOL_H
#include <string>
#include <cstdint>
#include <optional>
#include <vector>

// Simple message type enumeration for the protocol.
enum class MessageType {
    HELLO,
    COEFF,
    PUT,
    BAD_PUT,
    PENALTY,
    STATE,
    SCORING,
    ERROR
};

// Constants for message sizes.
constexpr std::size_t HELLO_SIZE = 6;
constexpr std::size_t COEFF_SIZE = 6;
constexpr std::size_t PUT_SIZE = 4;
constexpr std::size_t BAD_PUT_SIZE = 8;
constexpr std::size_t PENALTY_SIZE = 8;
constexpr std::size_t STATE_SIZE = 6;
constexpr std::size_t SCORING_SIZE = 8;

// Delimiter used in the protocol.
constexpr char const* DELIMITER = "\r\n";

// Extract message from buffer.
// std::nullopt if no complete message is found.
std::optional<std::string> extract_line(std::string& buffer);

// Extract the message type from a line.
MessageType get_message_type(std::string const& line);

// Validate the message based on its type.
// No description is provided if the validation fails.
bool validate_hello(std::string const& line);
bool validate_coeff(std::string const& line);
bool validate_put(std::string const& line);
bool validate_bad_put(std::string const& line);
bool validate_penalty(std::string const& line);
bool validate_state(std::string const& line);
bool validate_scoring(std::string const& line);

// Parse the message based on its type into appropriate data structures.
// Assumes that the message has been validated.
std::string parse_hello(std::string const& line);
std::vector<double> parse_coeff(std::string const& line);
std::pair<int16_t, double> parse_put(std::string const& line);
std::pair<int16_t, double> parse_bad_put(std::string const& line);
std::pair<int16_t, double> parse_penalty(std::string const& line);
std::vector<double> parse_state(std::string const& line);
std::vector<std::pair<std::string, double>> parse_scoring(std::string const& line);

// Build messages for sending.
std::string build_hello(std::string const& player_id);
std::string build_coeff(std::vector<double> const& coeffs);
std::string build_put(int16_t point, double value);
std::string build_bad_put(int16_t point, double value);
std::string build_penalty(int16_t point, double value);
std::string build_state(std::vector<double> const& values);
std::string build_scoring(std::vector<std::pair<std::string, double>> const& scoring);

#endif  // APPROX_PROTOCOL_H