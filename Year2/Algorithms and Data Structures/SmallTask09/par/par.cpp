#include <iostream>
#include <array>
#include <vector>
#include <limits>

constexpr std::size_t MAXN = 5e5 + 5;
constexpr std::size_t MAXLOG = 20;
constexpr std::size_t INF = std::numeric_limits<std::size_t>::max();

std::array<std::vector<std::size_t>, MAXN> tree;
std::array<std::size_t, MAXN> parent;
std::array<std::size_t, MAXN> distA;
std::array<std::size_t, MAXN> distB;
std::array<std::array<std::size_t, MAXLOG>, MAXN> ancestor;
std::array<std::size_t, MAXN> depth;

std::size_t A, B;

void find_diameter(std::size_t n) {
    std::vector<std::size_t> dist(n);
    std::size_t max_dist = 0;

    auto dfs = [&](auto&& self, std::size_t v, std::size_t father, auto& dist) -> void {
        for (auto u : tree[v]) {
            if (u == father) {
                continue;
            }
    
            dist[u] = dist[v] + 1;
            self(self, u, v, dist);
        }
    };

    dist[1] = 0;
    dfs(dfs, 1, 1, dist);

    for (std::size_t i = 1; i <= n; ++i ) {
        if (dist[i] > max_dist) {
            max_dist = dist[i];
            A = i;
        }
    }

    distA[A] = 0;
    dfs(dfs, A, A, distA);

    max_dist = 0;
    for (std::size_t i = 1; i <= n; ++i) {
        if (distA[i] > max_dist) {
            max_dist = distA[i];
            B = i;
        }
    }

    distB[B] = 0;
    dfs(dfs, B, B, distB);
}

void binary_lifting(std::size_t v, std::size_t father) {
    ancestor[v][0] = father;
    depth[v] = depth[father] + 1;

    for (std::size_t i = 1; i < MAXLOG; ++i) {
        ancestor[v][i] = ancestor[ancestor[v][i - 1]][i - 1];
    }

    for (auto u : tree[v]) {
        if (u == father) {
            continue;
        }

        binary_lifting(u, v);
    }
}

std::size_t solve(std::size_t x, std::size_t d, std::size_t farthest) {
    auto find_lca = [&](std::size_t u, std::size_t v) {
        if (depth[u] < depth[v]) {
            std::swap(u, v);
        }
    
        for (std::size_t i = MAXLOG - 1; i < MAXLOG; --i) {
            if (depth[ancestor[u][i]] >= depth[v]) {
                u = ancestor[u][i];
            }
        }
    
        if (u == v) { 
            return u;
        }
    
        for (std::size_t i = MAXLOG - 1; i < MAXLOG; --i) {
            if (ancestor[u][i] != ancestor[v][i]) {
                u = ancestor[u][i];
                v = ancestor[v][i];
            }
        }
    
        return parent[u];
    };

    auto uplift = [](std::size_t x, std::size_t d) {
        for (std::size_t i = MAXLOG - 1; i < MAXLOG; --i) {
            if (d & (1 << i)) {
                x = ancestor[x][i];
            }
        }

        return x;
    };

    std::size_t lca = find_lca(x, farthest);

    if (depth[x] - depth[lca] >= d) {
        return uplift(x, d);
    } else {
        return uplift(farthest, depth[farthest] + depth[x] - 2 * depth[lca] - d);
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t n, u, v, m, x, d, farthest;
    std::cin >> n;

    parent[1] = 1;
    for (std::size_t i = 1; i <= n; ++i) {
        std::cin >> u >> v;

        if (u != INF) {
            tree[i].push_back(u);
            tree[u].push_back(i);
            parent[u] = i;
        }

        if (v != INF) {
            tree[i].push_back(v);
            tree[v].push_back(i);
            parent[v] = i;
        }
    }

    depth[1] = INF;
    binary_lifting(1, 1);
    find_diameter(n);

    std::cin >> m;
    for (std::size_t i = 0; i < m; ++i) {
        std::cin >> x >> d;
        
        if (std::max(distA[x], distB[x]) < d) {
            std::cout << "-1\n";
            continue;
        }

        farthest = distA[x] > distB[x] ? A : B;
        std::cout << solve(x, d, farthest) << "\n";
    }
}
