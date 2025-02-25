#include <iostream>
#include <queue>

class AVL {
    class node {
    public:
        std::pair<int, int> data;
        node* left;
        node* right;
        int height;
        int onLeft;
        int lastChange;
    };
    node* root = NULL;

    int height(node* current) {
        if (current == NULL) return 0;
        return current->height;
    }

    node* rightRotation(node* current) {
        int b = current->onLeft;
        int a = current->left->onLeft;
        node* new_node = current->left;
        current->left = new_node->right;
        new_node->right = current;
        current->height = 1 + std::max(height(current->left), height(current->right));
        new_node->height = 1 + std::max(height(new_node->left), height(new_node->right));
        new_node->onLeft = a;
        current->onLeft = b - a - new_node->data.second;
        return new_node;
    }

    node* leftRotation(node* current) {
        int b = current->onLeft;
        int a = current->right->onLeft;
        node* new_node = current->right;
        current->right = new_node->left;
        new_node->left = current;
        current->height = 1 + std::max(height(current->left), height(current->right));
        new_node->height = 1 + std::max(height(new_node->left), height(new_node->right));
        new_node->onLeft = a + b + current->data.second;
        current->onLeft = b;
        return new_node;
    }

    node* insertUtility(node* current, std::pair<int, int> value, int kth) {
        if (current == NULL) {
            node* new_node = new node;
            new_node->data = value;
            new_node->right = NULL;
            new_node->left = NULL;
            new_node->height = 1;
            new_node->onLeft = 0;
            new_node->lastChange = 0;
            current = new_node;
            return new_node;
        }

        if (current-> onLeft >= kth) { 
            current->lastChange = -1;
            current->left = insertUtility(current->left, value, kth);
            current->onLeft += value.second;
        }
        else { 
            current->lastChange = 1;
            current->right = insertUtility(current->right, value, kth - current->onLeft - current->data.second);
        }

        current->height = 1 + std::max(height(current->left), height(current->right));
        int balance = height(current->left) - height(current->right);

        if (balance > 1) {

            if (current->left->lastChange == 1) {
                current->left = leftRotation(current->left);
                return rightRotation(current);
            }

            else {
                return rightRotation(current);
            }
        }
        else if (balance < -1) {

            if (current->right->lastChange == -1) {
                current->right = rightRotation(current->right);
                return leftRotation(current);
            }
            else {
                return leftRotation(current);
            }
        }
        return current;
    }

    node* findUtility(node* current, int kth) {
        if (current == NULL) {
            return current;
        }

        if (current->onLeft >= kth) {
            return findUtility(current->left, kth);
        }
        else if (current->onLeft + current->data.second >= kth) {
            return current;
        }
        else {
            return findUtility(current->right, kth - current->onLeft - current->data.second);
        }
    }

    std::pair<int, int> cutUtility (node* current, int kth) {
        if (current == NULL) {
            return {-1, -1};
        }

        if (current->onLeft >= kth) {
            auto res = cutUtility(current->left, kth);
            if (res.second != -1) {
                current->onLeft -= res.second;
            }
            return res;
        }
        else if (current->onLeft + current->data.second > kth) {
            int size = current->data.second;
            kth -= current->onLeft;
            current->data.second = kth;
            return { current->data.first, size - kth };
        }
        else {
            return cutUtility(current->right, kth - current->onLeft - current->data.second);
        }
    }

    void insert(std::pair<int, int> value, int kth) {
        root = insertUtility(root, value, kth);
    }

public:
    int find(int kth) {
        node* n = findUtility(root, kth);
        return n ? n->data.first : -1;
    }

    std::pair<int, int> cut(int kth) {
        return cutUtility(root, kth);
    }

    void insertMany(std::pair<int, int> value, int kth) {
        if (root == NULL) {
            insert(value, kth);
            return;
        }
        auto cutAmount = cutUtility(root, kth);
        insert(value, kth);
        if (cutAmount.second != -1) {
            insert(cutAmount, kth + value.second);
        }
    }
};

AVL avl;

int main(){
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int q, j, x, k, n = 0, w = 0;
    char c;
    std::cin >> q;
    for (int i = 0; i < q; ++i) {
        std::cin >> c;
        if (c == 'i') {
            std::cin >> j >> x >> k;
            j = (j + w) % (n + 1);
            avl.insertMany({x, k}, j);
            n += k;
        }
        else {
            std::cin >> j;
            j = (j + w) % n;
            w = avl.find(j + 1);
            std::cout << w << "\n";
        }
    }
}
