#include <iostream>
#include <vector>
#include <algorithm>

constexpr int MAXK = 10;
constexpr long long MOD = 1e9;

class PITree {
private:
    static constexpr size_t TREE_SIZE = 1 << 15; // 32 768 > 20 000 > 16 384

    struct node {
        long long value;
        size_t left;
        size_t right;
    };

    node TREE[2 * TREE_SIZE];

    void update(size_t id, long long val) {
        TREE[id].value = (TREE[id].value + val) % MOD;

        while (id > 0) {
            id /= 2;
            TREE[id].value = (TREE[2 * id].value + TREE[2 * id + 1].value) % MOD;
        }
    }

    long long read(size_t first, size_t last, size_t id) {
        if (TREE[id].left > last || TREE[id].right < first) return 0;
        if (first <= TREE[id].left && TREE[id].right <= last) return TREE[id].value;
        return (read(first, last, 2 * id) + read(first, last, 2 * id + 1)) % MOD;
    }
public:
    PITree() {
        for (size_t i = TREE_SIZE; i < 2 * TREE_SIZE; ++i) {
            TREE[i].left = TREE[i].right = i;
            TREE[i].value = 0;
        }

        for (int i = TREE_SIZE - 1; i > 0; --i) {
            TREE[i].left = TREE[2 * i].left;
            TREE[i].right = TREE[2 * i + 1].right;
            TREE[i].value = 0;
        }
    }

    void add(size_t id, long long x) {
        update(id + TREE_SIZE - 1, x % MOD);
    }

    long long get(size_t first, size_t last) {
        return read(first + TREE_SIZE - 1, last + TREE_SIZE - 1, 1) % MOD;
    }
};

PITree dp[MAXK + 1];

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, k;
    std::vector<int> perm;

    std::cin >> n >> k;

    perm.resize(n);
    for (int i = 0; i < n; ++i)
        std::cin >> perm[i];
    
    for (auto x : perm) {
        dp[1].add(x, 1);

        for (int length = 2; length <= k; ++length)
            dp[length].add(x, dp[length - 1].get(x + 1, n) % MOD);
    }
    
    std::cout << dp[k].get(1, n) % MOD << "\n";
}
