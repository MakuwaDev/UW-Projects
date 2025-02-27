#ifndef POLY_H
#define POLY_H

#include <array>
#include <type_traits>
#include <utility>
#include <cstddef>
#include <concepts>

template <typename T, std::size_t N = 0>
class poly;

namespace detail {
    // Type-trait to determine if given type is a polynomial
    template <typename T>
    struct is_poly : std::false_type {};

    template <typename T, std::size_t N>
    struct is_poly<poly<T, N>> : std::true_type {};

    template <typename T>
    constexpr bool is_poly_v = is_poly<T>::value;

    template <typename T1, typename T2>
    struct common_type {
        using type = std::common_type_t<T1, T2>;
    };

    // Helper structures for computing common types
    template <typename T1, std::size_t N1, typename T2, std::size_t N2>
    struct common_type<poly<T1, N1>, poly<T2, N2>> {
        using type = poly<std::common_type_t<T1, T2>, std::max(N1, N2)>;
    };

    template <typename T1, std::size_t N, typename T2>
    struct common_type<poly<T1, N>, T2> {
        using type = poly<std::common_type_t<T1, T2>, N>;
    };

    template <typename T1, std::size_t N, typename T2>
    struct common_type<T2, poly<T1, N>> {
        using type = poly<std::common_type_t<T1, T2>, N>;
    };

    // Helper structures for computing the resulting type of a polynomial multiplication
    template <typename T1, typename T2>
    struct poly_mul {
        using type = typename std::common_type_t<T1, T2>;
    };

    template <typename U, std::size_t N, std::size_t M, typename V>
    struct poly_mul<poly<U, N>, poly<V, M>> {
        using type = poly<typename poly_mul<U, V>::type, 0>;
    };

    template <typename U, std::size_t N, typename V, std::size_t M>
    requires (N > 0 && M > 0)
    struct poly_mul<poly<U, N>, poly<V, M>> {
        using type = poly<typename poly_mul<U, V>::type, N + M - 1>;
    };

    template <typename T1, typename T2>
    requires (!std::is_same_v<T1, std::decay_t<T1>> || !std::is_same_v<T2, std::decay_t<T2>>)
    struct poly_mul<T1, T2> : poly_mul<std::decay_t<T1>, std::decay_t<T2>> {};

    template <typename T1, typename T2>
    using poly_mul_t = typename poly_mul<T1, T2>::type;

    // Helper function to evaluate a coefficient
    template <typename Coefficient, typename... Args>
    constexpr auto evaluate(const Coefficient& coeff, const Args&... args) {
        if constexpr (is_poly_v<Coefficient>) {
            return coeff.at(args...);
        }
        else {
            return coeff;
        }
    }

} // namespace detail

// Definition of the poly class template
template <typename T, std::size_t N>
class poly {
private:
    std::array<T, N> coefficients{}; // Stores the coefficients of the polynomial

public:
    // Default ctor
    constexpr poly() = default;

    // Copy ctor for poly<U, M> where M <= N
    template <std::convertible_to<T> U, std::size_t M>
    requires (M <= N) 
    constexpr poly(const poly<U, M>& other) : coefficients{} {
        for (std::size_t i = 0; i < M; ++i) {
            coefficients[i] = other[i];
        }
    }

    // Move ctor for poly<U, M> where M <= N
    template <std::convertible_to<T> U, std::size_t M>
    requires (M <= N)
    constexpr poly(poly<U, M>&& other) : coefficients{} {
        for (std::size_t i = 0; i < M; ++i) {
            coefficients[i] = std::move(other[i]);
        }
    }

    // Conversion ctor for a single value
    template <std::convertible_to<T> U>
    requires (!detail::is_poly_v<std::decay_t<U>>)
    constexpr poly(U&& value) : coefficients{} {
        coefficients[0] = std::forward<U>(value);
    }

    // Variadic constructor with perfect forwarding
    template <typename... Args>
    requires (sizeof...(Args) >= 2) && (sizeof...(Args) <= N) && (std::convertible_to<Args, T> && ...)
    constexpr poly(Args&&... args)
        : coefficients{static_cast<T>(std::forward<Args>(args))...} {}

    // Copy assignment operator for poly<U, M> where M <= N
    template <std::convertible_to<T> U, std::size_t M>
    requires (M <= N)
    constexpr poly& operator=(const poly<U, M>& rhs) {
        for (std::size_t i = 0; i < M; ++i) {
            coefficients[i] = rhs[i];
        }

        for (std::size_t i = M; i < N; ++i) {
            coefficients[i] = T{};
        }

        return *this;
    }

    // Move assignment operator for poly<U, M> where M <= N
    template <std::convertible_to<T> U, std::size_t M>
    requires (M <= N)
    constexpr poly& operator=(poly<U, M>&& rhs) {
        for (std::size_t i = 0; i < M; ++i) {
            coefficients[i] = std::move(rhs[i]);
        }

        for (std::size_t i = M; i < N; ++i) {
            coefficients[i] = T{};
        }

        return *this;
    }

    // Operator += for poly<U, M> where M <= N
    template <std::convertible_to<T> U, std::size_t M>
    requires (M <= N)
    constexpr poly<T, N>& operator+=(const poly<U, M>& rhs) {
        for (std::size_t i = 0; i < M; ++i) {
            coefficients[i] += rhs[i];
        }

        return *this;
    }

    // Operator += for a constant value convertible to T
    template <std::convertible_to<T> U>
    constexpr poly<T, N>& operator+=(const U& rhs) {
        coefficients[0] += rhs;
        return *this;
    }

    // Operator -= for poly<U, M> where M <= N
    template <std::convertible_to<T> U, std::size_t M>
    requires (M <= N)
    constexpr poly<T, N>& operator-=(const poly<U, M>& rhs) {
        for (std::size_t i = 0; i < M; ++i) {
            coefficients[i] -= rhs[i];
        }

        return *this;
    }

    // Operator -= for a constant value convertible to T
    template <std::convertible_to<T> U>
    constexpr poly<T, N>& operator-=(const U& rhs) {
        coefficients[0] -= rhs;
        return *this;
    }

    // Operator *= for a constant value convertible to T
    template <std::convertible_to<T> U>
    constexpr poly<T, N>& operator*=(const U& rhs) {
        for (std::size_t i = 0; i < N; ++i) {
            coefficients[i] *= rhs;
        }

        return *this;
    }

    // Unary operator -
    constexpr poly<T, N> operator-() const {
        poly<T, N> result;
        for (std::size_t i = 0; i < N; ++i) {
            result[i] = -coefficients[i];
        }
        return result;
    }

    // Subscript operator (non-const)
    constexpr T& operator[](std::size_t i) {
        return coefficients[i];
    }

    // Subscript operator (const)
    constexpr const T& operator[](std::size_t i) const {
        return coefficients[i];
    }

    // 'At' method overload for empty evaluation
    // Returns the polynomial itself.
    constexpr auto at() const {
        return *this;
    }

    // Evaluation function 'at'
    // Evaluates the polynomial at a given point.
    template <typename First, typename... Rest>
    constexpr auto at(const First& first, const Rest&... rest) const {
        auto at_helper = [&]<std::size_t i>(const auto &self) {
            if constexpr (N == 0) {
                return T{};
            }
            else if constexpr (i == N - 1) {
                return detail::evaluate(coefficients[i], rest...);  // Evaluate the last term.
            }
            else {
                return detail::evaluate(coefficients[i], rest...) + 
                        first * self.template operator()<i + 1>(self);  // Accumulate terms recursively.
            }
        };

        return at_helper.template operator()<0>(at_helper); // Start evaluation from the first term.
    }

    // 'At' method overload for an input array
    // Calls variadic template version with array elements.
    template <typename U, std::size_t K>
    constexpr auto at(const std::array<U, K>& values) const {
        auto call_base = [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
            return at(values[Indices]...);  // Unpack array values into separate arguments.
        };

        return call_base(std::make_index_sequence<K>{});    // Generate index sequence for array unpacking.
    }

    // Size method
    constexpr std::size_t size() const {
        return coefficients.size();
    }
};

// Helper const-poly function
template <typename U>
requires detail::is_poly_v<std::decay_t<U>>
constexpr poly<std::decay_t<U>, 1> const_poly(U&& other) {
    poly<std::decay_t<U>, 1> result;
    result[0] = std::forward<U>(other);
    return result;
}

// Binary operator + for two polynomials.
template <typename T, std::size_t N, typename U, std::size_t M>
constexpr poly<std::common_type_t<T, U>, std::max(N, M)> operator+(const poly<T, N>& lhs, const poly<U, M>& rhs) {
    poly<std::common_type_t<T, U>, std::max(N, M)> result{lhs};
    result += rhs;
    return result;
}

// Binary operator +: left hand side argument is a polynomial
template <typename T, std::size_t N, typename U>
constexpr poly<std::common_type_t<T, U>, N> operator+(const poly<T, N>& lhs, const U& rhs) {
    if constexpr (N != 0) {
        poly<std::common_type_t<T, U>, N> result{lhs};
        result += rhs;
        return result;
    }
    else {
        return lhs;
    }
}

// Binary operator +: right hand side argument is a polynomial
template <typename T, std::size_t N, typename U>
constexpr poly<std::common_type_t<T, U>, N> operator+(const U& lhs, const poly<T, N>& rhs) {
    if constexpr (N != 0) {
        poly<std::common_type_t<T, U>, N> result{rhs};
        result += lhs;
        return result;
    }
    else {
        return rhs;
    }
}

// Binary operator - for two polynomials
template <typename T, std::size_t N, typename U, std::size_t M>
constexpr poly<std::common_type_t<T, U>, std::max(N, M)> operator-(const poly<T, N>& lhs, const poly<U, M>& rhs) {
    poly<std::common_type_t<T, U>, std::max(N, M)> result{lhs};
    result += -rhs;
    return result;
}

// Binary operator -: left hand side argument is a polynomial
template <typename T, std::size_t N, typename U>
constexpr poly<std::common_type_t<T, U>, N> operator-(const poly<T, N>& lhs, const U& rhs) {
    if constexpr (N != 0) {
        poly<std::common_type_t<T, U>, N> result{lhs};
        result -= rhs;
        return result;
    }
    else {
        return lhs;
    }
}

// Binary operator -: right hand side argument is a polynomial
template <typename T, std::size_t N, typename U>
constexpr poly<std::common_type_t<T, U>, N> operator-(const U& lhs, const poly<T, N>& rhs) {
    if constexpr (N != 0) {
        poly<std::common_type_t<T, U>, N> result{-rhs};
        result += lhs;
        return result;
    }
    else {
        return rhs;
    }
}

// Binary operator * for two polynomials
template <typename T, std::size_t N, typename U, std::size_t M>
constexpr detail::poly_mul_t<poly<T, N>, poly<U, M>> operator*(const poly<T, N>& lhs, const poly<U, M>& rhs) {
    detail::poly_mul_t<poly<T, N>, poly<U, M>> result{};
    
    if constexpr (N == 0 || M == 0) {
        return result;  // Any zero-sized polynomial results in an empty zero-sized polynomial.
    }

    // Multiply adequate coefficients.
    for (std::size_t i = 0; i < N; ++i) {
        for (std::size_t j = 0; j < M; ++j) {
            detail::poly_mul_t<T, U> temp = lhs[i] * rhs[j];
            result[i + j] += temp;
        }
    }

    return result;
}

// Binary operator *: left hand side argument is a polynomial
template <typename T, std::size_t N, std::convertible_to<T> U>
constexpr detail::poly_mul_t<poly<T, N>, U> operator*(const poly<T, N>& lhs, const U& rhs) {
    if constexpr (N != 0) {
        detail::poly_mul_t<poly<T, N>, U> result = lhs;
        result *= rhs;
        return result;
    }
    else {
        return lhs;
    }
}

// Binary operator *: right hand side argument is a polynomial
template <typename T, std::size_t N, std::convertible_to<T> U>
constexpr detail::poly_mul_t<poly<T, N>, U> operator*(const U& lhs, const poly<T, N>& rhs) {
    if constexpr (N != 0) {
        detail::poly_mul_t<poly<T, N>, U> result = rhs;
        result *= lhs;
        return result;
    }
    else {
        return rhs;
    }
}

// Cross function for a polynomial and another polynomial when the first polynomial's coefficients 
// are not themselves polynomials.
template <typename T, std::size_t N, typename U, std::size_t M>
requires (!detail::is_poly_v<T>)
constexpr poly<std::common_type_t<T, poly<U, M>>, N> cross(const poly<T, N>& lhs, const poly<U, M>& rhs) {
    poly<std::common_type_t<T, poly<U, M>>, N> result{};

    for (std::size_t i = 0; i < N; ++i) {
        result[i] = lhs[i] * rhs;   // Multiply each coefficient of lhs with the entire rhs polynomial.
    }

    return result;
}

// Recursive cross product for polynomials where coefficients may themselves be polynomials.
template <typename T, std::size_t N, typename U, std::size_t M>
constexpr auto cross(const poly<T, N>& lhs, const poly<U, M>& rhs) -> poly<decltype(cross(lhs[0], rhs)), N> {
    poly<decltype(cross(lhs[0], rhs)), N> result{};

    for (std::size_t i = 0; i < N; ++i) {
        result[i] = cross(lhs[i], rhs);     // Recursively compute the result.
    }

    return result;
}

// Deduction guide for poly
template <typename... Args>
poly(Args&&...) -> poly<std::common_type_t<std::decay_t<Args>...>, sizeof...(Args)>;


// std::common type specialization for poly
namespace std {
    template <typename T1, typename T2>
    requires (detail::is_poly_v<decay_t<T1>> || detail::is_poly_v<decay_t<T2>>)
    struct common_type<T1, T2> : conditional_t<is_same_v<decay_t<T1>, T1> && is_same_v<decay_t<T2>, T2>,
        detail::common_type<T1, T2>, detail::common_type<decay_t<T1>, decay_t<T2>>> {};
}

#endif // POLY_H
