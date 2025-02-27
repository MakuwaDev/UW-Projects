#include <iostream>
#include <string>
#include <regex>
#include <array>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <numeric>

namespace {
    // Type aliases for clarity, consistency and ease of modification.
    using line_counter_t = u_int32_t;
    using medal_count_t = u_int32_t;
    using score_t = u_int64_t;
    using weight_t = u_int32_t;

    // Global map of country names to an array of 3 medals (gold, silver, bronze).
    std::unordered_map<std::string, std::array<medal_count_t, 3>> medals;

    // Global line number counter for error tracking.
    line_counter_t line_number = 0;

    // Extracts medal weights used to calculate the score from given string.
    std::vector<weight_t> get_weights(const std::string &line) {
        std::vector<weight_t> ret;
        std::string::const_iterator b = line.begin();

        // Parse each weight from the line.
        while (b != line.end()) {
            ++b;

            // Find adequate iterator and parse next weight into the result vector.
            std::string::const_iterator e = std::find(b, line.end(), ' ');
            ret.push_back(std::stoi(std::string(b, e)));

            b = e;
        }

        return ret;
    }

    // Prints an error message with the current line number.
    // Called on invalid input or incorrect medal operation.
    void print_error(void) {
        std::cerr << "ERROR " << line_number << "\n";
    }

    // Adds a medal if the medal type is non-zero, otherwise initializes the country with no medals.
    void give_medal(const std::string &line) {
        // Extract country name and medal type from given string.
        std::string country(line.begin(), line.end() - 2);
        size_t medal_type = line.back() - '0';

        // Initialize country with empty medal counts or increment the respective medal count.
        if (!medal_type) {
            medals[country];
        }
        else {
            ++medals[country][medal_type - 1];
        }
    }

    // Removes a medal from the country if possible, otherwise prints an error.
    void take_medal(const std::string &line) {
        // Extract country name and medal type from given string.
        std::string country(line.begin() + 1, line.end() - 2);
        size_t medal_type = line.back() - '0';

        // If country doesn't exist or there are no medals of that type, print an error.
        if (!medals.contains(country) || !medals[country][medal_type - 1]) {
            print_error();
            return;
        }

        // Decrement the respective medal count.
        --medals[country][medal_type - 1];
    }

    // Prints the ranking based on the weighted sum of medals.
    void print_classification(const std::string &line) {
        // Get medal weights and declare a container for keeping scores.
        std::vector<weight_t> weights = get_weights(line);
        std::vector<std::pair<std::string, score_t>> scores;

        // Define variables used to count country ranks and handle ties.
        score_t last_score;
        size_t current_rank;
        
        // If no medals are present, no classification is printed.
        if (medals.empty()) {
            return;
        }

        // Calculate scores for each country using std::inner_product with the provided weights.
        for (const auto &[country, country_medals] : medals)
            scores.push_back({country,
                std::inner_product(weights.begin(), weights.end(), country_medals.begin(), score_t(0))});

        // Sort descendingly by score and then lexicographically by country name.
        std::sort(scores.begin(), scores.end(), [](const auto &a, const auto &b) {
            return (a.second > b.second) || (a.second == b.second && a.first < b.first);
        });

        // Print countries with their rank, handling ties.
        for (size_t i = 0; i < scores.size(); ++i) {
            const auto &[country, score] = scores[i];

            // On the first iteration, set the initial rank and last_score.
            // For subsequent iterations, adjust rank if there was no tie.
            if (!i) {
                current_rank = 1;
                last_score = score;
            } 
            else if (score != last_score) {
                last_score = score;
                current_rank = i + 1;
            }

            // Print rank and country.
            std::cout << current_rank << ". " << country << "\n";
        }
    }

    // Validates input and processes given query by calling adequate functions.
    void process_query(const std::string &line) {
        // Regex patterns for different query types.
        static const std::regex give_medal_pattern("^[A-Z]( *[a-zA-Z]+)+ [0123]$");
        static const std::regex take_medal_pattern("^-[A-Z]( *[a-zA-Z]+)+ [123]$");
        static const std::regex print_classification_pattern("^=([1-9]\\d{0,5} ){2}[1-9]\\d{0,5}$");

        // Increment line number for error reporting.
        ++line_number;

        // Match the line against the patterns and call the appropriate function.
        if (std::regex_match(line, give_medal_pattern)) {
            give_medal(line);
        }
        else if (std::regex_match(line, take_medal_pattern)) {
            take_medal(line);
        }
        else if (std::regex_match(line, print_classification_pattern)) {
            print_classification(line);
        }
        else {
            print_error();
        }
    }
} // namespace

int main() {
    std::string line;

    // Read input lines until EOF and process each query.
    while (std::getline(std::cin, line))
        process_query(line);

    return 0;
}
