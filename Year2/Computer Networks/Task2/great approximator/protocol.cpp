#include "protocol.h"

#include <regex>
#include <sstream>
#include <iomanip>

// Extract message from buffer.
// std::nullopt if no complete message is found.
std::optional<std::string> extract_line(std::string& buffer) {
    std::size_t pos = buffer.find(DELIMITER);
    if (pos == std::string::npos) {
        return std::nullopt;
    }

    std::string line = buffer.substr(0, pos);
    buffer.erase(0, pos + 2);
    return line;
}

// Extract the message type from a line.
MessageType get_message_type(std::string const& line) {
    if (line.starts_with("HELLO")) {
        return MessageType::HELLO;
    } else if (line.starts_with("COEFF")) {
        return MessageType::COEFF;
    } else if (line.starts_with("PUT")) {
        return MessageType::PUT;
    } else if(line.starts_with("BAD_PUT")) {
        return MessageType::BAD_PUT;
    } else if (line.starts_with("PENALTY")) {
        return MessageType::PENALTY;
    } else if (line.starts_with("STATE")) {
        return MessageType::STATE;
    } else if(line.starts_with("SCORING")) {
        return MessageType::SCORING;
    } else {
        return MessageType::ERROR;
    }
}

// Validate a HELLO message.
bool validate_hello(std::string const& line) {
    static const std::regex pattern(R"(^HELLO [a-zA-Z0-9]+$)");
    return std::regex_match(line, pattern);
}

// Validate a COEFF message.
bool validate_coeff(std::string const& line) {
    static const std::regex pattern(R"(^COEFF( -?\d+(\.\d{0,7})?)+$)");
    return std::regex_match(line, pattern);
}

// Validate a PUT message.
bool validate_put(std::string const& line) {
    static const std::regex pattern(R"(^PUT \d+ -?\d+(\.\d{0,7})?$)");
    return std::regex_match(line, pattern);
}

// Validate a BAD_PUT message.
bool validate_bad_put(std::string const& line) {
    static const std::regex pattern(R"(^BAD_PUT \d+ -?\d+(\.\d{0,7})?$)");
    return std::regex_match(line, pattern);
}

// Validate a PENALTY message.
bool validate_penalty(std::string const& line) {
    static const std::regex pattern(R"(^PENALTY \d+ -?\d+(\.\d{0,7})?$)");
    return std::regex_match(line, pattern);
}

// Validate a STATE message.
bool validate_state(std::string const& line) {
    static const std::regex pattern(R"(^STATE( -?\d+(\.\d{0,7})?)+$)");
    return std::regex_match(line, pattern);
}

// Validate a SCORING message.
bool validate_scoring(std::string const& line) {
    static const std::regex pattern(R"(^SCORING ([a-zA-Z0-9]+ \d+(\.\d{0,7})?)+$)");
    return std::regex_match(line, pattern);
}

// Parse a HELLO message.
std::string parse_hello(std::string const& line) {
    return line.substr(6);
}

// Parse a COEFF message.
std::vector<double> parse_coeff(std::string const& line) {
    std::istringstream iss(line.substr(COEFF_SIZE));
    std::vector<double> coeffs;
    double coeff;

    while (iss >> coeff) {
        coeffs.push_back(coeff);
    }

    return coeffs;
}

// Parse a PUT message.
std::pair<int16_t, double> parse_put(std::string const& line) {
    std::istringstream iss(line.substr(PUT_SIZE));
    std::pair<int16_t, double> res;
    iss >> res.first >> res.second;
    return res;
}

// Parse BAD_PUT message.
std::pair<int16_t, double> parse_bad_put(std::string const& line) {
    std::istringstream iss(line.substr(BAD_PUT_SIZE));
    std::pair<int16_t, double> res;
    iss >> res.first >> res.second;
    return res;
}

// Prase a PENALTY message.
std::pair<int16_t, double> parse_penalty(std::string const& line) {
    std::istringstream iss(line.substr(PENALTY_SIZE));
    std::pair<int16_t, double> res;
    iss >> res.first >> res.second;
    return res;
}

// Parse a STATE message.
std::vector<double> parse_state(std::string const& line) {
    std::istringstream iss(line.substr(STATE_SIZE));
    std::vector<double> values;
    double val;

    while (iss >> val) {
        values.push_back(val);
    }

    return values;
}

// Parse a SCORING message.
std::vector<std::pair<std::string, double>> parse_scoring(std::string const& line) {
    std::istringstream iss(line.substr(SCORING_SIZE));

    std::vector<std::pair<std::string, double>> scoring;
    std::string player_id;
    double score;
    while (iss >> player_id >> score) {
        scoring.emplace_back(player_id, score);
    }

    return scoring;
}

// Build a HELLO message.
std::string build_hello(std::string const& player_id) {
    std::ostringstream oss;
    oss << "HELLO " << player_id << DELIMITER;
    return oss.str();
}

// Build a COEFF message.
std::string build_coeff(std::vector<double> const& coeffs) {
    std::ostringstream oss;
    oss << "COEFF";

    for (const auto& coeff : coeffs) {
        oss << " " << std::fixed << std::setprecision(7) << coeff;
    }

    oss << DELIMITER;
    return oss.str();
}

// Build a PUT message.
std::string build_put(int16_t point, double value) {
    std::ostringstream oss;
    oss << "PUT " << static_cast<int>(point) << " " 
        << std::fixed << std::setprecision(7) << value 
        << DELIMITER;
    return oss.str();
}

// Build a BAD_PUT message.
std::string build_bad_put(int16_t point, double value) {
    std::ostringstream oss;
    oss << "BAD_PUT " << static_cast<int>(point) << " " 
        << std::fixed << std::setprecision(7) << value 
        << DELIMITER;
    return oss.str();
}

// Build a PENALTY message.
std::string build_penalty(int16_t point, double value) {
    std::ostringstream oss;
    oss << "PENALTY " << static_cast<int>(point) << " " 
        << std::fixed << std::setprecision(7) << value 
        << DELIMITER;
    return oss.str();
}

// Build a STATE message.
std::string build_state(std::vector<double> const& values) {
    std::ostringstream oss;
    oss << "STATE";

    for (const auto& val : values) {
        oss << " " << std::fixed << std::setprecision(7) << val;
    }

    oss << DELIMITER;
    return oss.str();
}

// Build a SCORING message.
std::string build_scoring(std::vector<std::pair<std::string, double>> const& scoring) {
    std::ostringstream oss;
    oss << "SCORING";

    for (const auto& [player_id, score] : scoring) {
        oss << " " << player_id << " " << std::fixed << std::setprecision(7) << score;
    }

    oss << DELIMITER;
    return oss.str();
}