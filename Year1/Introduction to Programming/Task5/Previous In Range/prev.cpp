//Author: Krzysztof Hałubek
#include <vector>
#include <memory>
#include <limits>
#include <cmath>

#include "prev.h"

namespace {
    class persistent_tree {
    private:
        struct node;
        typedef std::shared_ptr<node> node_ptr;

        struct node {
            node_ptr left, right;
            int val;

            node() {
                left = right = nullptr;
                val = -1;
            }
        };

        std::vector<node_ptr> trees; // root pointers for all versions of the tree
        static constexpr std::pair<int, int> maxRange = {std::numeric_limits<int>::min(), std::numeric_limits<int>::max()}; // interval covered by the whole tree

        // Builds a new tree utilizing the previous one and setting value of element id as x
        void build(node_ptr curr, node_ptr prev, int x, int id, std::pair<int, int> range) {
            if(range.first == range.second) { // == id, we traversed down to the leaf
                curr->val = x;
                return;
            }

            int mid = (int)std::floor(((double)range.first + (double)range.second) / 2);
            if(id <= mid) { // id is in the left subtree
                curr->left = node_ptr(new node());
                if(prev) curr->right = prev->right;
                build(curr->left, prev ? prev->left : nullptr, x, id, {range.first, mid});
            }
            else { // id is in the right subtree
                curr->right = node_ptr(new node());
                if(prev) curr->left = prev->left;
                build(curr->right, prev ? prev->right : nullptr, x, id, {mid + 1, range.second});
            }

            int l = curr->left ? curr->left->val : -1; // largest element in the left subtree, -1 if no such exists
            int r = curr->right ? curr->right->val : -1; // largest element in the right subtree, -1 if no such exists
            curr->val = std::max(l, r);
        }

        // Finds the largest element from a given interval
        int query(node_ptr n, int low, int top, std::pair<int, int> range) {
            if(low > range.second || top < range.first) return -1; // [low, top] and [range.first, range.second] do not intersect
            if(low <= range.first && range.second <= top) return n->val; // [low, top] contains [range.first, range.second]

            int mid = (int)std::floor(((double)range.first + (double)range.second) / 2);
            int l = n->left ? query(n->left, low, top, {range.first, mid}) : -1; // if it exists, go to the left subtree
            int r = n->right ? query(n->right, low, top, {mid + 1, range.second}) : -1; // if it exists, go to the right subtree
            return std::max(l, r);
        }

        // removes the subtree rooted in n
        void erase(node_ptr n) {
            if(n->left) {
                erase(n->left);
                n->left = nullptr;
            }
            if(n->right) {
                erase(n->right);
                n->right = nullptr;
            }

        }

    public:
        // updates the persistent tree by setting value x in element id
        void update(int x, int id) {
            node_ptr root(new node());
            build(root, trees.empty() ? nullptr : trees.back(), x, id, maxRange);
            trees.push_back(root);
        }

        // returns the maximal value from the interval [low, top] in the ith tree
        int get(int i, int low, int top) {
            if(trees.empty()) return -1;
            return query(trees[i], low, top, maxRange);
        }

        persistent_tree(const std::vector<int>& seq) {
            for(size_t i = 0; i < seq.size(); ++i)
                update((int)i, seq[i]);
        }

        ~persistent_tree() {
            while(!trees.empty()) {
                erase(trees.back());
                trees.back().reset();
                trees.pop_back();
            }
        }
    };

    size_t size; // current length of the sequence
    std::unique_ptr<persistent_tree> sequence;
}

void init(const std::vector<int> &seq) {
    sequence = std::make_unique<persistent_tree>(seq);
    size = seq.size();
}

int prevInRange(int i, int lo, int hi) {
    return sequence->get(i, lo, hi);
}

void pushBack(int value) {
    sequence->update((int)size++, value);
}

void done() {
    sequence.reset();
}
