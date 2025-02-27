export module main;

import <iostream>;
import <cstdint>;
import <sstream>;
import <memory>;
import <queue>;
import <regex>;
import <array>;

import counter_collection;

struct RegexEntry {
    std::regex pattern;
    char id;
};

static constexpr std::size_t regex_count = 6ULL;

static const std::array<RegexEntry, regex_count> regex_entries = {{
    {std::regex{R"(^M ([1-9]\d*|0) ([1-9]\d*|0) ([1-9]\d*|0)$)"}, 'M'},
    {std::regex{R"(^F ([1-9]\d*|0) ([1-9]\d*|0)$)"}, 'F'},
    {std::regex{R"(^G ([1-9]\d*|0) ([1-9]\d*|0)$)"}, 'G'},
    {std::regex{R"(^D ([1-9]\d*|0)$)"}, 'D'},
    {std::regex{R"(^P ([1-9]\d*|0)$)"}, 'P'},
    {std::regex{R"(^A ([1-9]\d*|0)$)"}, 'A'}
}};

static counter_collection counters{};

static counter::events_queue events_q{};

static void print_error(uint64_t line_number) {
    std::cerr << "ERROR " << line_number << "\n";
}

static void process_line(const std::string& line, uint64_t line_number) {
    std::istringstream ss{line};
    std::string c_string, p_string, m_string, t_string;
    uint64_t c_parameter, p_parameter, m_parameter, t_parameter;
    
    for (const auto& entry : regex_entries) {
        if (std::regex_match(line, entry.pattern)) {
            ss >> c_string; // Discard first letter of input

            switch (entry.id) {
                case 'M':
                    ss >> c_string >> p_string >> m_string;

                    try {
                        c_parameter = std::stoull(c_string);
                        p_parameter = std::stoull(p_string);
                        m_parameter = std::stoull(m_string);
                    } catch (...) {
                        print_error(line_number);
                        return;
                    }

                    if (!counters.insert(
                            c_parameter, 
                            std::make_unique<counter_modulo>(
                                c_parameter, 
                                p_parameter, 
                                events_q, 
                                m_parameter
                            )
                        )) {
                        print_error(line_number);
                    }

                    break;
                case 'F':
                    ss >> c_string >> p_string;

                    try {
                        c_parameter = std::stoull(c_string);
                        p_parameter = std::stoull(p_string);
                    } catch (...) {
                        print_error(line_number);
                        return;
                    }

                    if (!counters.insert(
                            c_parameter, 
                            std::make_unique<counter_fibonacci>(
                                c_parameter, 
                                p_parameter, 
                                events_q
                            )
                        )) {
                        print_error(line_number);
                    }   

                    break;
                case 'G':
                    ss >> c_string >> p_string;

                    try {
                        c_parameter = std::stoull(c_string);
                        p_parameter = std::stoull(p_string);
                    } catch (...) {
                        print_error(line_number);
                        return;
                    }

                    if (!counters.insert(
                        c_parameter,
                        std::make_unique<counter_geometric>(
                            c_parameter,
                            p_parameter,
                            events_q
                        )
                    )) {
                        print_error(line_number);
                    }

                    break;
                case 'D':
                    ss >> c_string;

                    try {
                        c_parameter = std::stoull(c_string);
                    } catch (...) {
                        print_error(line_number);
                        return;
                    }

                    if (!counters.erase(c_parameter)) {
                        print_error(line_number);
                    }

                    break;
                case 'P':
                    ss >> c_string;

                    try {
                        c_parameter = std::stoull(c_string);
                    } catch (...) {
                        print_error(line_number);
                        return;
                    }

                    if (!counters.print_state(c_parameter)) {
                        print_error(line_number);
                    }   

                    break;
                case 'A':
                    ss >> t_string;

                    try {
                        t_parameter = std::stoull(t_string);
                    } catch (...) {
                        print_error(line_number);
                        return;
                    }

                    counters.send_signals(t_parameter);
                    while (!events_q.empty()) {
                        auto top = events_q.top();
                        events_q.pop();
                        std::cout << "E " << top.second << " " << top.first << '\n';
                    }

                    break;
            }

            return;
        }
    }

    print_error(line_number);
}

int main() {
    uint64_t i = 1ULL; // line counter
    std::string line;

    while (std::getline(std::cin, line)) {
        process_line(line, i);
        i++;
    }
    return 0;
}