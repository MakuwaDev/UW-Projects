#include <iostream>

constexpr size_t MAXN = 1000;
constexpr size_t MAXK = 12;

long long board[MAXN + 5][MAXK + 5];
long long dp[2][MAXK + 5][(1 << MAXK)];

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, k;

    std::cin >> n >> k;

    for (int j = 0; j < k; ++j)
        for (int i = 0; i < n; ++i)
            std::cin >> board[i][j];
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < k; ++j)
            for (int p = 0; p < (1 << k); ++p) {
                if ((p & (1 << j)) != 0) {
                    int q = p - (1 << j);
                    dp[i % 2][j + 1][q] = std::max(dp[i % 2][j + 1][q], std::max(dp[i % 2][j][p], dp[i % 2][j][q]));
                }
                else {
                    int q = p + (1 << j);
                    dp[i % 2][j + 1][q] = std::max(dp[i % 2][j + 1][q], std::max(dp[i % 2][j][p] + board[i][j] + board[i + 1][j], dp[i % 2][j][p]));

                    if (j < k - 1 && (p & (1 << (j + 1))) == 0) {
                        int q = p + (1 << (j + 1));
                        dp[i % 2][j + 1][q] = std::max(dp[i % 2][j + 1][q], std::max(dp[i % 2][j][p] + board[i][j] + board[i][j + 1], dp[i % 2][j][p]));
                    }
                }
            }

        for (int p = 0; p < (1 << k); ++p)
            dp[(i + 1) % 2][0][p] = dp[i % 2][k][p];
    }

    std::cout << dp[n % 2][0][0] << "\n";
}
