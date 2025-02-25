#include <iostream>
#include <vector>
#include <array>
#include <queue>
#include <algorithm>
#include <limits>

struct isle {
    int x;
    int y;
    int id;
};

constexpr auto axes = {&isle::x, &isle::y};
constexpr int MAXN = 2e5 + 5;
constexpr int INF = std::numeric_limits<int>::max();

std::array<isle, MAXN> isles{};
std::array<std::vector<std::pair<int, int>>, MAXN> graph{};
std::array<int, MAXN> distance{};

inline int dist(isle const& lhs, isle const& rhs) {
    int res = INF;
    for (auto axis : axes) {
        res = std::min(res, std::abs(lhs.*axis - rhs.*axis));
    }

    return res;
}

void generateEdges(int n, int isle::* member) {
    std::sort(isles.begin(), isles.begin() + n, [member](isle const& lhs, isle const& rhs) {
        return lhs.*member < rhs.*member;
    });

    for (int i = 0; i < n - 1; ++i) {
        isle const& a = isles[i];
        isle const& b = isles[i + 1];

        int d{dist(a, b)};
        graph[a.id].push_back({d, b.id});
        graph[b.id].push_back({d, a.id});
    }
}

void dijkstra(int start, int n) {
    std::fill_n(distance.begin(), n, INF);
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> q;

    q.push({0, start});
    while (!q.empty()) {
        int v = q.top().second;
        int d = q.top().first;
        q.pop();

        if (distance[v] != INF) {
            continue;
        }

        distance[v] = d;
        for (auto x : graph[v]) {
            q.push({d + x.first, x.second});
        }
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n{};
    std::cin >> n;

    for (int i = 0; i < n; ++i) {
        std::cin >> isles[i].x >> isles[i].y;
        isles[i].id = i;
    }

    for (auto axis : axes) {
        generateEdges(n, axis);
    }

    dijkstra(0, n);
    std::cout << distance[n - 1] << "\n";
}
