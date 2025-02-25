#include <iostream>
#include <vector>
#include <cstddef>
#include <algorithm>
#include <numeric>
#include <utility>

template <typename Iterator>
class KMR {
public:
    using ValueType = typename Iterator::value_type;
    using IDType = int;

private:
    std::size_t size;
    std::size_t lg;
    std::vector<std::vector<IDType>> ids;

    void radixSort(std::vector<std::pair<IDType, std::pair<IDType, IDType>>>& pairs) {
        auto bucketSort = [&](auto selector) {
            std::vector<size_t> count(size, 0);
            std::vector<std::pair<IDType, std::pair<IDType, IDType>>> out(pairs.size());

            for (auto const& p : pairs) {
                ++count[selector(p)];
            }

            for (size_t i = 1; i < count.size(); ++i) {
                count[i] += count[i - 1];
            }

            for (auto it = pairs.rbegin(); it != pairs.rend(); ++it) {
                size_t index = --count[selector(*it)];
                out[index] = *it;
            }

            pairs = std::move(out);
        };

        bucketSort([](auto const& p){ return p.second.second; });
        bucketSort([](auto const& p){ return p.second.first; });
    }

public:
    KMR(Iterator const& begin, Iterator const& end) : ids{} {
        size = std::distance(begin, end);
        lg = std::__lg(size) + 3;
        std::vector<IDType> run(size);
        std::vector<std::pair<IDType, std::pair<IDType, IDType>>> pairs;

        auto assignIDs = [&](auto loopCondition, auto keyExtractor, auto indexExtractor, std::size_t log) {
            IDType f{};
            for (std::size_t i = 0; loopCondition(i); ++i) {
                auto currKey = keyExtractor(i);
                auto prevKey = i ? keyExtractor(i - 1) : currKey; 
                auto currIndex = indexExtractor(i);
                ids[currIndex][log] = !i || currKey == prevKey ? f : ++f;
            }
        };

        ids.resize(size);
        for (std::size_t i = 0; i < size; ++i) {
            ids[i].resize(lg);
        }
        
        std::iota(run.begin(), run.end(), IDType{});
        std::sort(run.begin(), run.end(), [&](std::size_t i, std::size_t j) {
            return begin[i] < begin[j];
        });

        assignIDs(
            [&](std::size_t i) { return i < size; },
            [&](std::size_t i) { return begin[run[i]]; },
            [&](std::size_t i) { return run[i]; },
            0
        );

        for (std::size_t log = 1; log < lg; ++log) {
            pairs.clear();
            std::size_t d = 1 << (log - 1);

            for (std::size_t i = 0; i + 2 * d - 1 < size; ++i) {
                pairs.push_back({(IDType)i, {ids[i][log - 1], ids[i + d][log - 1]}});
            }
            radixSort(pairs);

            assignIDs(
                [&](std::size_t i) { return i + 2 * d - 1 < size; },
                [&](std::size_t i) { return pairs[i].second; },
                [&](std::size_t i) { return pairs[i].first; },
                log
            );
        }
    }

    std::pair<IDType, IDType> operator()(std::size_t i, std::size_t j) const {
        std::size_t d = j - i, log = d ? std::__lg(d) : 0;
        return {ids[i][log], ids[j - (1 << log) + 1][log]};
    }

    int operator()(std::size_t a, std::size_t b, std::size_t c, std::size_t d) const {
        int swapped = 1;
        if (b - a > d - c) {
            std::swap(a, c);
            std::swap(b, d);
            swapped = -1;
        }
        
        auto l = this->operator()(a, b), r = this->operator()(c, c + b - a);

        if (b - a == d - c && l == r) {
            return 0;
        }
        else {
            return swapped * (l <= r ? -1 : 1);
        }
    }
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::size_t n, m, a, b, c, d;
    std::string s;
    std::cin >> n >> m >> s;

    KMR kmr(s.begin(), s.end());

    for (std::size_t i = 0; i < m; ++i) {
        std::cin >> a >> b >> c >> d;
        auto r = kmr(a - 1, b - 1, c - 1, d - 1);
        switch (r)
        {
        case -1:
            std::cout << "<\n";
            break;
        case 1:
            std::cout << ">\n";
            break;
        default:
            std::cout << "=\n";
            break;
        }
    }
}
