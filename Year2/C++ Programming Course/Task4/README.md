# Multivariate Polynomial Ring
## Short task summary/translation:

This project involves implementing a multivariate polynomial ring using a template class:

```cpp
template <typename T, std::size_t N = 0> class poly;
```

An object of this class represents a single-variable polynomial of the form:  

    a₀ + a₁x + a₂x² + … + aₙ₋₁xⁿ⁻¹

where coefficients aᵢ are of type T. The parameter N represents the size of the polynomial (degree + 1).

To support multivariate polynomials, the type T can itself be a polynomial (poly). For example:

```cpp
poly<poly<poly<int, 3>, 2>, 4>
```
represents a 3-variable polynomial over integers, where the outermost polynomial is in x1, its coefficients are polynomials in x2, and so on.
## Implemented Features
### Constructors

    Default constructor: Creates a polynomial equal to zero.
    Copy and move constructors for poly<U, M>, where M <= N, and U is convertible to T.
    Conversion constructor: Allows initialization from a single value convertible to T.
    Variadic constructor: Allows initialization from multiple coefficients (up to N).

Additionally, a function const_poly(p) constructs a polynomial of size 1, where p is its only coefficient.
### Assignment Operators

    Copy and move assignment for poly<U, M>, where M <= N and U is convertible to T.

### Arithmetic Operators

    In-place operations (+=, -=, *=) for polynomials or values convertible to T.
    Binary arithmetic (+, -, *) with support for:
        Polynomial + scalar
        Scalar + polynomial
        Polynomial + polynomial
    Unary - operator.

The result type is automatically deduced using std::common_type_t<U, V>, ensuring the minimal required polynomial size.
Indexing and Evaluation

    operator[](size_t i): Returns a reference to coefficient aiai​.
    at(...): Evaluates the polynomial at given points.
        Supports partial evaluation (fewer arguments than variables).
        Can accept an std::array for batch evaluation.

Other Methods

    size(): Returns the polynomial's size (N).

    cross(p, q): Computes the cross product of two polynomials:
    cross(p,q)(x1,...,xn,y1,...,ym)=p(x1,...,xn)⋅q(y1,...,ym)
    cross(p,q)(x1​,...,xn​,y1​,...,ym​)=p(x1​,...,xn​)⋅q(y1​,...,ym​)

Additional Requirements

    All methods and constructors are constexpr, allowing compile-time evaluation.
    Helper definitions are hidden inside a detail namespace.
