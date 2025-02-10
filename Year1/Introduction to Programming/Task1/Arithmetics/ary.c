#include <stdlib.h>
#include <stdio.h>

#include "ary.h"

bool iszero(double x) 
{ 
    return fabs(x) < 1e-12; 
}

bool equal(double x, double y)
{
    return fabs(x - y) < 1e-12;
}

void swap(wartosc *a, wartosc *b)
{
    wartosc tmp = *a;
    *a = *b;
    *b = tmp;
}

double multiply(double a, double b)
{
    if(isnan(a) || isnan(b)) return NAN;
    else if(iszero(a) || iszero(b)) return 0.0;
    else return a * b;
}

wartosc NAW() // Not A Wartosc
{
    wartosc zwracana;

    zwracana.lower = zwracana.upper = NAN;
    zwracana.disjoint = NAN;
    
    return zwracana;
}  

wartosc wartosc_dokladnosc(double x, double p)
{
    wartosc zwracana;

    if(iszero(x)) zwracana.lower = zwracana.upper = 0.0;
    else if(x > 0.0)
    {
        zwracana.lower = x - (x * p / 100.0);
        zwracana.upper = x + (x * p / 100.0);
    }
    else 
    {
        zwracana.lower = x + (x * p / 100.0);
        zwracana.upper = x - (x * p / 100.0);
    }
    zwracana.disjoint = false;

    return zwracana;
}

wartosc wartosc_od_do(double x, double y)
{
    wartosc zwracana;

    zwracana.lower = x;
    zwracana.upper = y;
    zwracana.disjoint = false;

    return zwracana;
}

wartosc wartosc_dokladna(double x)
{
    wartosc zwracana;

    zwracana.lower = x;
    zwracana.upper = x;
    zwracana.disjoint = false;

    return zwracana;
}

bool in_wartosc(wartosc w, double x)
{
    if(!w.disjoint) return ((x > w.lower || equal(x, w.lower)) && ((x < w.upper ) || equal(x, w.upper)));
    else return ((x < w.lower || equal(x, w.lower)) || (x > w.upper || equal(x, w.upper)));
}

double min_wartosc(wartosc w)
{   
    if(isnan(w.lower)) return NAN;
    else if(!w.disjoint) return w.lower;
    else return -HUGE_VAL;
}

double max_wartosc(wartosc w)
{
    if(isnan(w.upper)) return NAN;
    else if(!w.disjoint) return w.upper;
    else return HUGE_VAL;
}

double sr_wartosc(wartosc w)
{
   return (min_wartosc(w) + max_wartosc(w)) / 2.0;
}

wartosc plus_coherent_coherent(wartosc a, wartosc b)
{
    wartosc zwracana;

    zwracana.lower = a.lower + b.lower;
    zwracana.upper = a.upper + b.upper;
    zwracana.disjoint = false;

    return zwracana;
}

wartosc plus_disjoint_disjoint(wartosc a, wartosc b)
{
    wartosc zwracana;
    
    // dla spojnosci kodu, funkcja przyjmuje parametry, chociaz ich nie potrzebuje; nastepne dwie linijki istnieja tylko po to, by pozbyc sie warningow
    zwracana.disjoint = a.disjoint;
    zwracana.disjoint = b.disjoint;

    zwracana.lower = -HUGE_VAL;
    zwracana.upper = HUGE_VAL;
    zwracana.disjoint = false;

    return zwracana;
}

wartosc plus_disjoint_coherent(wartosc a, wartosc b)
{
    wartosc zwracana;

    if(b.disjoint) swap(&a, &b); // zakladamy, ze a jest rozlaczne

    zwracana = a;
    zwracana.lower += b.upper;
    zwracana.upper += b.lower;

    if(zwracana.lower > zwracana.upper || equal(zwracana.lower, zwracana.upper)) // zmiana na zbior liczb rzeczywistych
    {
        zwracana.lower = -HUGE_VAL;
        zwracana.upper = HUGE_VAL;
        zwracana.disjoint = false;
    }

    return zwracana;
}

wartosc plus(wartosc a, wartosc b)
{
    if((isnan(a.lower) && isnan(a.upper)) || (isnan(b.lower) && isnan(b.upper))) return NAW();
    else if(!a.disjoint && !b.disjoint) return plus_coherent_coherent(a, b);
    else if(a.disjoint && b.disjoint) return plus_disjoint_disjoint(a, b);
    else return plus_disjoint_coherent(a, b);
}

wartosc minus(wartosc a, wartosc b)
{
    wartosc przeciwna; // a - b = a + (-b)

    if((isnan(a.lower) && isnan(a.upper)) || (isnan(b.lower) && isnan(b.upper))) return NAW();

    przeciwna.lower = -b.upper;
    przeciwna.upper = -b.lower;
    przeciwna.disjoint = b.disjoint;

    return plus(a, przeciwna);
}

wartosc razy_coherent_coherent(wartosc a, wartosc b)
{
    wartosc zwracana;

    zwracana.lower = fmin(fmin(multiply(a.lower, b.lower), multiply(a.lower, b.upper)), fmin(multiply(a.upper, b.lower), multiply(a.upper, b.upper))); // dolny kres iloczynów to minimum z iloczynów kresów
    zwracana.upper = fmax(fmax(multiply(a.lower, b.lower), multiply(a.lower, b.upper)), fmax(multiply(a.upper, b.lower), multiply(a.upper, b.upper))); // górny kres iloczynów to maximum z iloczynów kresów
    zwracana.disjoint = false;

    return zwracana;
}

wartosc razy_disjoint_disjoint(wartosc a, wartosc b)
{
    wartosc zwracana;

    if(a.lower > 0.0 || a.upper < 0.0 || b.lower > 0.0 || b.upper < 0.0 || iszero(a.lower) || iszero(a.upper) || iszero(b.lower) || iszero(b.upper)) // ktorykolwiek z przedzialow zawiera 0
    {
        zwracana.lower = -HUGE_VAL;
        zwracana.upper = HUGE_VAL;
        zwracana.disjoint = false;
    }
    else // 0 jest pomiedzy przedzialami w obu przypadkach
    {
        zwracana.lower = fmax(multiply(a.lower, b.upper), multiply(a.upper, b.lower));
        zwracana.upper = fmin(multiply(a.lower, b.lower), multiply(a.upper, b.upper));
        zwracana.disjoint = true;
    }

    return zwracana;
}

wartosc razy_disjoint_coherent(wartosc a, wartosc b)
{
    wartosc zwracana;

    if(b.disjoint) swap(&a, &b); // zakladamy, ze a jest rozlaczne

    if(iszero(b.lower) && iszero(b.upper)) return b; // b = [0, 0]
    else if(b.lower > 0.0) // b > 0
    {
        zwracana.lower = fmax(multiply(a.lower, b.lower), multiply(a.lower, b.upper));
        zwracana.upper = fmin(multiply(a.upper, b.lower), multiply(a.upper, b.upper));
        zwracana.disjoint = true;
    }
    else if(b.upper < 0.0) // b < 0
    {
        zwracana.lower = fmax(multiply(a.upper, b.upper), multiply(a.upper, b.lower));
        zwracana.upper = fmin(multiply(a.lower, b.upper), multiply(a.lower, b.lower));
        zwracana.disjoint = true;
    }
    else // b zawiera 0
    {
        zwracana.lower = -HUGE_VAL;
        zwracana.upper = HUGE_VAL;
        zwracana.disjoint = false;
    }

    return zwracana;
}

wartosc razy(wartosc a, wartosc b)
{
    if((isnan(a.lower) && isnan(a.upper)) || (isnan(b.lower) && isnan(b.upper))) return NAW();
    else if(!a.disjoint && !b.disjoint) return razy_coherent_coherent(a, b);
    else if(a.disjoint && b.disjoint) return razy_disjoint_disjoint(a, b);
    else return razy_disjoint_coherent(a, b);
}

wartosc podzielic(wartosc a, wartosc b)
{
    wartosc odwrotna; // a / b = a * (1/b)

    if((isnan(a.lower) && isnan(a.upper)) || (isnan(b.lower) && isnan(b.upper))) return NAW();

    if(!b.disjoint)
    {
        if(iszero(b.lower) && iszero(b.upper)) odwrotna = NAW(); // [0, 0]
        else if(iszero(b.upper)) // [x, 0]
        {
            odwrotna.lower = -HUGE_VAL;
            odwrotna.upper = 1 / b.lower;
            odwrotna.disjoint = false;
        }
        else if(iszero(b.lower)) // [0, x]
        {
            odwrotna.lower = 1 / b.upper;
            odwrotna.upper = HUGE_VAL;
            odwrotna.disjoint = false;
        }
        else if(b.lower < 0.0 && 0.0 < b.upper) // [x, y], gdzie x < 0 < y
        {
            odwrotna.lower = 1 / b.lower;
            odwrotna.upper = 1 / b.upper;
            odwrotna.disjoint = true;
        }
        else // [x, y] nie zawiera zera
        {
            odwrotna.lower = 1 / b.upper;
            odwrotna.upper = 1 / b.lower;
            odwrotna.disjoint = false;
        }
    }
    else
    {
        if(iszero(b.lower)) // [-inf, 0] u [y, inf]
        {
            odwrotna.lower = -HUGE_VAL;
            odwrotna.upper = 1 / b.upper;
            odwrotna.disjoint = false;
        }
        else if(iszero(b.upper)) // [-inf, x] u [0, inf]
        {
            odwrotna.lower = 1 / b.lower;
            odwrotna.upper = HUGE_VAL;
            odwrotna.disjoint = false;
        }
        else if(b.lower < 0.0 && 0.0 < b.upper) // [-inf, x] u [y, inf], gdzie x < 0 < y
        {
            odwrotna.lower = 1 / b.lower;
            odwrotna.upper = 1 / b.upper;
            odwrotna.disjoint = false;
        }
        else if(b.lower > 0 || b.upper < 0) // [-inf, x] u [y, inf] gdzie 0 < x lub [-inf, x] u [y, inf] gdzie y < 0
        {
            odwrotna.lower = 1 / b.upper;
            odwrotna.upper = 1 / b.lower;
            odwrotna.disjoint = true;
        }
    }

    return razy(a, odwrotna);
}
