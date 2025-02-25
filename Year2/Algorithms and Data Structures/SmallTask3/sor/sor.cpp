#include <iostream>
#include <vector>
#include <array>

constexpr size_t MAX_N = 1010;
constexpr long long MOD = 1e9;

std::array<std::array<std::array<long long, 2>, MAX_N>, MAX_N> dp;

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t n;
    std::vector<long long> sequence;

    std::cin >> n;

    sequence.resize(n);
    for(size_t i = 0; i < n; ++i)
        std::cin >> sequence[i];
    
    if(n == 1) {
        std::cout << "1\n";
        return 0;
    }

    for(size_t i = 0; i < n; ++i)
        dp[i][i][0] = dp[i][i][1] = 1;

    for(size_t b = 0; b + 1 < n; ++b) {
        dp[b][b + 1][0] = sequence[b] < sequence[b + 1] ? 1 : 0;
        dp[b][b + 1][1] = sequence[b + 1] > sequence[b] ? 1 : 0;
    }
    
    for(size_t len = 3; len <= n; ++len)
        for(size_t b = 0, e; b + len - 1 < n; ++b) {
            e = b + len - 1;

            dp[b][e][0] = (dp[b][e][0] + (sequence[b] < sequence[e] ? dp[b + 1][e][1] : 0)) % MOD;
            dp[b][e][0] = (dp[b][e][0] + (sequence[b] < sequence[b + 1] ? dp[b + 1][e][0] : 0)) % MOD;

            dp[b][e][1] = (dp[b][e][1] + (sequence[e] > sequence[b] ? dp[b][e - 1][0] : 0)) % MOD;
            dp[b][e][1] = (dp[b][e][1] + (sequence[e] > sequence[e - 1] ? dp[b][e - 1][1] : 0)) % MOD;
        }

    std::cout << (dp[0][n - 1][0] + dp[0][n - 1][1]) % MOD << "\n";
    return 0;
}
