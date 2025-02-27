export module counters;

import <cstdint>;
import <queue>;
import <iostream>;

export class counter {
public:
    using events_queue = std::priority_queue<std::pair<uint64_t, uint64_t>, std::vector<std::pair<uint64_t, uint64_t>>, std::greater<std::pair<uint64_t, uint64_t>>>;

protected:
    uint64_t id;
    uint64_t current;
    uint64_t ignored; // [0; p] effective signal when ignored changes from p to 0
    uint64_t p;
    events_queue& events;

    std::pair<uint64_t, uint64_t> flatten_p(uint64_t signals) {
        uint64_t diff = p - ignored;
        if (signals <= diff) {
            ignored += signals;
            return {0ULL, 0ULL};
        } else {
            signals -= (diff + 1); // ignored = 0

            if (p == UINT64_MAX) {
                ignored = signals;
                return {1ULL, diff + 1ULL};
            } else {
                ignored = signals % (p + 1ULL);
                return {signals / (p + 1ULL) + 1ULL, diff + 1ULL};
            }
        }
    }

public:
    counter(uint64_t id, uint64_t p, events_queue& q) : id(id), current(0ULL), ignored(p), p(p), events(q) {}
    virtual ~counter() {};

    virtual void receive_signals(uint64_t signals_count) = 0;

    void print_state() {
        std::cout << "C " << id << ' ' << current << '\n';
    }
};

export class counter_modulo : public counter {
protected:
    uint64_t modulo;

public:
    counter_modulo(uint64_t id, uint64_t p, events_queue& q, uint64_t m) : counter(id, p, q), modulo(m) {}

    ~counter_modulo() override = default;

    void receive_signals(uint64_t signals_count) final override {
        auto [effective_signals_count, last_signal] = flatten_p(signals_count);
        
        uint64_t left = modulo - current;
        
        if (effective_signals_count <= left) {
            current += effective_signals_count;
            return;
        }

        if (modulo == UINT64_MAX) {
            effective_signals_count -= left;
            current = modulo;
            if (p == UINT64_MAX) {
                return;
            } else {
                current = effective_signals_count - 1ULL;
                events.push({last_signal + left * (p + 1ULL), id});           
            }
        } else {
            if (p == UINT64_MAX && effective_signals_count == 1ULL) {
                if (current == modulo) {
                    current = 0ULL;
                    events.push({last_signal, id});
                } else
                    current++;

            } else {
                uint64_t i = 0ULL;
                while (effective_signals_count > modulo - current) {
                    uint64_t diff = modulo - current + 1ULL;
                    effective_signals_count -= diff;
                    current = 0ULL;

                    // -1 in the first run of a loop to take into account 
                    // that the first signal was after 'last_signal' time
                    last_signal = last_signal + (diff - (i == 0ULL ? 1ULL : 0ULL)) * (p + 1ULL);
                    events.push({last_signal, id});
                    ++i;
                }

                current = effective_signals_count;
            }
        }
    }
};

export class counter_fibonacci : public counter {
    constexpr static const uint64_t last_fib = 12200160415121876738ULL;
    
    uint64_t fib1, fib2;

public:
    counter_fibonacci(uint64_t id, uint64_t p, events_queue& q) : counter(id, p, q), fib1(0ULL), fib2(1ULL) {}
    
    ~counter_fibonacci() override = default;

    void receive_signals(uint64_t signals_count) final override {
        if (current == UINT64_MAX)
            return;

        auto [effective_signals_count, last_signal] = flatten_p(signals_count);

        if (p == UINT64_MAX && effective_signals_count == 1ULL) {
            uint64_t next_fib = fib1 + fib2;
            current++;

            if (current == next_fib) {
                fib1 = fib2;
                fib2 = next_fib;
                events.push({last_signal, id});
            }
            return;
        }

        uint64_t i = 0ULL;
        while (effective_signals_count) {
            if (fib2 < last_fib) {
                uint64_t next_fib = fib1 + fib2;
                uint64_t diff = std::min(effective_signals_count, next_fib - current);
                effective_signals_count -= diff;
                current += diff;

                if (current == next_fib) {
                    fib1 = fib2;
                    fib2 = next_fib;

                    last_signal = last_signal + (diff - (i == 0ULL ? 1ULL : 0ULL)) * (p + 1ULL);
                    events.push({last_signal, id});
                }
                ++i;
            } else { // no more fibs
                current += std::min(effective_signals_count, UINT64_MAX - current);
                effective_signals_count = 0;
            }
        }
    }
};

export class counter_geometric : public counter {
    constexpr static const uint64_t start = 9ULL;
    constexpr static const uint64_t end = 1'000'000'000'000ULL - 1ULL;

    uint64_t current_mod;

    uint64_t next_mod(uint64_t current_mod) {
        return current_mod * 10LL + 9LL;
    }

public:
    counter_geometric(uint64_t id, uint64_t p, events_queue& q) : counter(id, p, q), current_mod(start) {}
    ~counter_geometric() override = default;

    void receive_signals(uint64_t signals_count) final override {
        auto [effective_signals_count, last_signal] = flatten_p(signals_count);
        uint64_t i = 0ULL;
        while (effective_signals_count > current_mod - current) {
            uint64_t diff = current_mod - current + 1;

            effective_signals_count -= diff;
            current = 0ULL;
            last_signal = last_signal + (diff - (i == 0ULL ? 1ULL : 0ULL)) * (p + 1ULL);
            events.push({last_signal, id});

            if (current_mod == end) {
                current_mod = start;
            } else {
                current_mod = next_mod(current_mod);
            }
            ++i;
        }

        current += effective_signals_count;
    }
};
