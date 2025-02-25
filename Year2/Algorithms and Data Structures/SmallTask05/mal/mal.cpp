#include <iostream>

constexpr size_t MAXN = 1e6 + 6;
constexpr size_t MAXM = 1e6 + 6;

class II_Tree {
private:
    static constexpr size_t TREESIZE = 1 << 20;
    
    struct node {
        size_t left;
        size_t right;
        int sum;
        bool lazy;
        bool covered;
    };

    node TREE[2 * TREESIZE];

    void push(size_t id) {
        TREE[2 * id].covered = TREE[2 * id + 1].covered = TREE[id].covered;
        TREE[2 * id].sum = TREE[2 * id + 1].sum = TREE[id].covered ? (TREE[id].right - TREE[id].left + 1) / 2 : 0;
        TREE[2 * id].lazy = TREE[2 * id + 1].lazy = true;
    }

    void update(size_t first, size_t last, size_t id, bool value) {
        if (TREE[id].lazy) {
            TREE[id].lazy = false;
            if (id < TREESIZE)
                push(id);
        }

        if (TREE[id].right < first || TREE[id].left > last)
            return;

        if (first <= TREE[id].left && TREE[id].right <= last) {
            TREE[id].lazy = true;
            TREE[id].covered = value;
            TREE[id].sum = value ? TREE[id].right - TREE[id].left + 1 : 0;

            while (id > 0) {
                id /= 2;
                TREE[id].sum = TREE[2 * id].sum + TREE[2 * id + 1].sum;
            }

            return;
        }

        update(first, last, 2 * id, value);
        update(first, last, 2 * id + 1, value);
    }

    int read(size_t first, size_t last, size_t id) {
        if (TREE[id].lazy) {
            TREE[id].lazy = false;
            if (id < TREESIZE)
                push(id);
        }

        if (TREE[id].right < first || TREE[id].left > last)
            return 0;

        if (first <= TREE[id].left && TREE[id].right <= last)
            return TREE[id].sum;

        return read(first, last, 2 * id) + read(first, last, 2 * id + 1);
    }
public:
    II_Tree() {
        for (size_t i = TREESIZE; i < 2 * TREESIZE; ++i) {
            TREE[i].left = TREE[i].right = i;
            TREE[i].sum = TREE[i].lazy = TREE[i].covered;
        }

        for (size_t i = TREESIZE - 1; i > 0; --i) {
            TREE[i].left = TREE[2 * i].left;
            TREE[i].right = TREE[2 * i + 1].right;
            TREE[i].sum = TREE[i].lazy = TREE[i].covered;
        }
    }
    
    void set(size_t first, size_t last, bool value) {
        update(first + TREESIZE - 1, last + TREESIZE - 1, 1, value);
    }

    int get(size_t first, size_t last) {
        return read(first + TREESIZE - 1, last + TREESIZE - 1, 1);
    }
} tree;

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m, a, b;
    char c;

    std::cin >> n >> m;

    for (int i = 0; i < m; ++i) {
        std::cin >> a >> b >> c;

        tree.set(a, b, c == 'B');
        std::cout << tree.get(1, n) << "\n";
    }
}
