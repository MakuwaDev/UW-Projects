#include <iostream>
#include <array>
#include <vector>
#include <queue>

constexpr int MAXN = 1e5 + 5;
constexpr int MAXM = 5e5 + 5;

std::array<long long, MAXN> pro;
std::array<int, MAXN> in;
std::array<std::vector<int>, MAXN> graph;

auto pqCmp = [](int lhs, int rhs) { return pro[lhs] > pro[rhs]; };

std::priority_queue<int, std::vector<int>, decltype(pqCmp)> available(pqCmp);

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m, k, a, b, done = 0;
    long long res = 0;
    std::cin >> n >> m >> k;

    for (int i = 1; i <= n; ++i)
        std::cin >> pro[i];
    
    for (int i = 0; i < m; ++i) {
        std::cin >> a >> b;
        graph[b].push_back(a);
        ++in[a];
    }

    for (int i = 1; i <= n; ++i)
        if (!in[i]) available.push(i);
    
    while (!available.empty()) {
        int x = available.top();
        available.pop();
        
        res = std::max(pro[x], res);
        if (++done == k) break;

        for (auto v : graph[x]) {
            if (!--in[v]) available.push(v);
        }
    }

    std::cout << res << "\n";
}
