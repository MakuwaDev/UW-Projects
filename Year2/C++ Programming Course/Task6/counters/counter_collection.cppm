export module counter_collection;

import <unordered_map>;
import <memory>;

export import counters;

export class counter_collection {
public:
    using counter_map = std::unordered_map<uint64_t, std::unique_ptr<counter>>;

    counter_collection() : counters{} {};
    ~counter_collection() = default;

    bool print_state(uint64_t key) {
        auto it = counters.find(key);
        if (it == counters.end())
            return false;
        
        it->second->print_state();
        return true;
    }

    size_t erase(uint64_t key) {
        return counters.erase(key);
    }

    bool insert(uint64_t key, std::unique_ptr<counter>&& counter_ptr) {
        if (counters.contains(key))
            return false;

        counters[key] = std::move(counter_ptr);
        return true;
    }

    void send_signals(uint64_t t_parameter) {
        for (auto &entry : counters)
            entry.second->receive_signals(t_parameter);
    }

private:
    counter_map counters;
};