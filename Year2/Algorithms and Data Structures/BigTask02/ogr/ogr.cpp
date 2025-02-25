#include <concepts>
#include <array>
#include <tuple>
#include <algorithm>
#include <iostream>

namespace {

template <typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::same_as<T>;
    { a += b };
    { a + b == b + a};
};

template <typename T>
concept Comparable = requires(T a, T b) {
    { a <= b } -> std::convertible_to<bool>;
};

template <typename T>
concept DefaultConstructible = std::default_initializable<T>;

template <typename T>
concept IntervalTreeCompatible = 
    Addable<T> && 
    Comparable<T> && 
    DefaultConstructible<T>;

} // namespace

template <typename T, std::size_t N>
requires IntervalTreeCompatible<T>
class II_tree {
private:
    struct Node {
        std::size_t mid_length;
        std::size_t left_length;
        std::size_t right_length;
        std::size_t interval_left;
        std::size_t interval_right;
        T min_left;
        T max_right;
        T lazy;
    };
    
    static constexpr std::size_t TREESIZE = []() constexpr {
        std::size_t size = 1;
        while (size < N) {
            size <<= 1;
        }
        return size;
    }();

    std::array<Node, 2 * TREESIZE> tree;

    void add(std::size_t id, T value) {
        tree[id].min_left += value;
        tree[id].max_right += value;
        tree[id].lazy += value;
    }

    void push(std::size_t id) {
        add(2 * id, tree[id].lazy);
        add(2 * id + 1, tree[id].lazy);
    }

    void update(std::size_t first, std::size_t last, std::size_t id, T value) {
        if (tree[id].lazy != T{}) {
            if (id < TREESIZE) {
                push(id);
            }
            tree[id].lazy = T{};
        }

        if (tree[id].interval_right < first || tree[id].interval_left > last) {
            return;
        }

        if (first <= tree[id].interval_left && tree[id].interval_right <= last) {
            add(id, value);
            
            while (id > 0) {
                id /= 2;
                tree[id].min_left = tree[2 * id].min_left;
                tree[id].max_right = tree[2 * id + 1].max_right;
                tree[id].mid_length = std::max({
                    tree[2 * id].mid_length,
                    tree[2 * id + 1].mid_length,
                    tree[2 * id].max_right <= tree[2 * id + 1].min_left ?
                        tree[2 * id].right_length + tree[2 * id + 1].left_length : 0
                });

                bool left_condition = 
                    tree[2 * id].max_right <= tree[2 * id + 1].min_left &&
                    tree[2 * id].left_length == tree[2 * id].interval_right - tree[2 * id].interval_left + 1;

                tree[id].left_length = left_condition ? tree[id].mid_length : tree[2 * id].left_length;

                bool right_condition =
                    tree[2 * id].max_right <= tree[2 * id + 1].min_left &&
                    tree[2 * id + 1].right_length == tree[2 * id + 1].interval_right - tree[2 * id + 1].interval_left + 1;

                tree[id].right_length = right_condition ? tree[id].mid_length : tree[2 * id + 1].right_length;
            }

            return;
        }

        update(first, last, 2 * id, value);
        update(first, last, 2 * id + 1, value);
    }

    std::tuple<std::size_t, std::size_t, std::size_t> read(std::size_t first, std::size_t last, std::size_t id) {
        if (tree[id].lazy != T{}) {
            if (id < TREESIZE) {
                push(id);
            }
            tree[id].lazy = T{};
        }

        if (tree[id].interval_right < first || tree[id].interval_left > last) {
            return {0, 0, 0};
        }
        else if (first <= tree[id].interval_left && tree[id].interval_right <= last) {
            return {tree[id].left_length, tree[id].mid_length, tree[id].right_length};
        }
        else {
            auto [left_left, left_mid, left_right] = read(first, last, 2 * id);
            auto [right_left, right_mid, right_right] = read(first, last, 2 * id + 1);

            std::size_t left, right, mid = std::max({
                left_mid,
                right_mid,
                tree[2 * id].max_right <= tree[2 * id + 1].min_left ? left_right + right_left : 0
            });

            bool left_condition = 
                tree[2 * id].max_right <= tree[2 * id + 1].min_left &&
                left_left == tree[2 * id].interval_right - std::max(first, tree[2 * id].interval_left) + 1;

            left =  left_condition ? mid : left_left;

            bool right_condition = 
                tree[2 * id].max_right <= tree[2 * id + 1].min_left &&
                right_right == std::min(last, tree[2 * id + 1].interval_right) - tree[2 * id + 1].interval_left + 1;

            right = right_condition ? mid : right_right;

            return {left, mid, right};
        }
    }

public:
    II_tree() {
        for (std::size_t i = TREESIZE; i < 2 * TREESIZE; ++i) {
            tree[i].mid_length = tree[i].left_length = tree[i].right_length = 1;
            tree[i].interval_left = tree[i].interval_right = i;
            tree[i].min_left = tree[i].max_right = tree[i].lazy = T{};
        }

        for (std::size_t i = TREESIZE - 1; i > 0; --i) {
            tree[i].mid_length = tree[2 * i].mid_length + tree[2 * i + 1].mid_length;
            tree[i].left_length = tree[2 * i].left_length + tree[ 2 * i + 1].left_length;
            tree[i].right_length = tree[2 * i].right_length + tree[2 * i + 1].right_length;
            tree[i].interval_left = tree[2 * i].interval_left;
            tree[i].interval_right = tree[2 * i + 1].interval_right;
            tree[i].min_left = tree[i].max_right = tree[i].lazy = T{};
        }
    }

    void increase(std::size_t first, std::size_t last, T value) {
        update(first + TREESIZE - 1, last + TREESIZE - 1, 1, value);
    }

    std::size_t get(std::size_t first, std::size_t last) {
        return std::get<1>(read(first + TREESIZE - 1, last + TREESIZE - 1, 1));
    }
};

constexpr std::size_t MAXN = 5e5 + 5;

II_tree<std::size_t, MAXN> tree;

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::size_t n, m, a, b, k;
    char c;

    std::cin >> n >> m;

    for (std::size_t i = 0; i < m; ++i) {
        std::cin >> c;

        if (c == 'N') {
            std::cin >> a >> b >> k;
            tree.increase(a, b, k);
        }
        else if (c == 'C') {
            std::cin >> a >> b;
            std::cout << tree.get(a, b) << '\n';
        }
    }
}
