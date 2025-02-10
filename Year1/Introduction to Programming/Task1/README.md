# Arithmetic of Approximate Values
## Short task summary/tranlsation:  
In measurements of physical quantities, the results are always subject to some error, e.g., 5m ± 10%. We treat each such approximate value as a set of possible values. Implement a package for arithmetic operations on such approximate values that follows the provided interface in ary.h. Specifically, the package should include:
## Type Definition:

    struct wartosc

## Constructors:

    wartosc_dokladnosc(x, p) returns x ± p% (for p > 0)
    wartosc_od_do(x, y) returns (x + y) / 2 ± (y - x) / 2 (for x < y)
    wartosc_dokladna(x) returns x ± 0

## Selectors:

    in_wartosc(x, y) checks if value x can be equal to y
    min_wartosc(x) returns the lower bound of possible values of x (or -∞ if x is not bounded below, or nan if x is empty)
    max_wartosc(x) returns the upper bound of possible values of x (or ∞ if x is not bounded above, or nan if x is empty)
    sr_wartosc(x) returns the arithmetic mean of min_wartosc(x) and max_wartosc(x) (or nan if the bounds are not finite)

## Modifiers:

    plus(a, b) = { x + y : in_wartosc(a, x) ∧ in_wartosc(b, y) }
    minus(a, b) = { x - y : in_wartosc(a, x) ∧ in_wartosc(b, y) }
    razy(a, b) = { x * y : in_wartosc(a, x) ∧ in_wartosc(b, y) }
    podzielic(a, b) = { x / y : in_wartosc(a, x) ∧ in_wartosc(b, y) }

## Assumptions:

    All arguments of type double are real numbers (i.e., different from HUGE_VAL, -HUGE_VAL, and NAN).
    If the result is not a real number, it should be HUGE_VAL, -HUGE_VAL, or NAN accordingly.

## Additional Information:

    Operations on approximate values are assumed to be monotonic due to the containment of possible value sets.
    The operations should close the result sets  
    (i.e., if the result is an open interval, it is treated as a closed interval).
    Some edge cases in floating-point operations may arise,  
    such as dividing by zero or dealing with positive/negative zeros.
