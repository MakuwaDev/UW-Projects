#ifndef FLIST
#define FLIST

#include <functional>
#include <string>
#include <sstream>
#include <ranges>

namespace flist::detail {

constexpr auto create_impl() {
    return []([[maybe_unused]] auto f, auto a) {
        return a;
    };
}

template <typename First, typename... Rest>
constexpr auto create_impl(First first, Rest... rest) {
    return [first, rest...](auto f, auto a) {
        return f(first, create_impl(rest...)(f, a));
    };
}

template <typename Range, typename Iterator, typename F, typename A>
constexpr auto of_range_impl(const Range& range, Iterator it, F f, A a) {
    if (it == std::end(range)) {
        return a;
    } else {
        auto x = *it;
        return f(x, of_range_impl(range, std::next(it), f, a));
    }
}

constexpr auto string_builder = [](auto x, std::reference_wrapper<std::string> s_ref) {
    std::ostringstream os;
    os << x;
    auto& s = s_ref.get();
    s = os.str() + ";" + s;
    return s_ref;
};

template <typename F, typename A>
constexpr auto snoc = [](auto x, auto l) -> std::function<A(F, A)> {
    return std::function<A(F, A)>([l, x](F f, A a) {
        return l(f, f(x, a));
    });
};

} // namespace flist::detail

namespace flist {

constexpr auto empty = []([[maybe_unused]] auto f, auto a) {
    return a;
};

constexpr auto cons = [](auto x, auto l) {
    return [x, l](auto f, auto a) {
        return f(x, l(f, a));
    };
};

constexpr auto create = [](auto... args) {
    return detail::create_impl(args...);
};

constexpr auto of_range = [](auto r) {
    if constexpr (std::ranges::bidirectional_range<decltype(r)>) {
        return [r](auto f, auto a) {
            return detail::of_range_impl(r, std::begin(r), f, a);
        };
    } else {
        auto& rr = r.get();
        return [rr](auto f, auto a) {
            return detail::of_range_impl(rr, std::begin(rr), f, a);
        };
    }
};

constexpr auto concat = [](auto l, auto k) {
    return [l, k](auto f, auto a) {
        return l(f, k(f, a));
    };
};

constexpr auto rev = [](auto l) {
    return [l](auto f, auto a) {
        using F = decltype(f);
        using A = decltype(a);
        return l(detail::snoc<F, A>, std::function<A(F, A)>(empty))(f, a);
    };
};

constexpr auto map = [](auto m, auto l) {
    return [m, l](auto f, auto a) {
        return l([m, f](auto x, auto a) { return f(m(x), a); }, a);
    };
};

constexpr auto filter = [](auto p, auto l) {
    return [p, l](auto f, auto a) {
        return l([p, f](auto x, auto a) { return p(x) ? f(x, a) : a; }, a);
    };
};

constexpr auto flatten = [](auto l) {
    return [l](auto f, auto a) {
        return l([f](auto x, auto a) { return x(f, a); }, a);
    };
};

constexpr auto as_string = [](const auto& l) -> std::string {
    std::string result("");
    l(detail::string_builder, std::ref(result));

    if (!result.empty()) {
        result.pop_back();
    }

    return "[" + result + "]";
};

} // namespace flist

#endif
