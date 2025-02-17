#ifndef UTILS_H
#define UTILS_H

/** MACROS TO SHORTEN LIBRARY IMPLEMENTATION **/

#define max(a, b) a > b ? a : b

// Macro to shorten error handling.
#define error_check(cond, err, ret)     \
    do {                                \
        if((cond)) {                    \
            errno = (err);              \
            return (ret);               \
        }                               \
    } while(0)                          

// Macro to swap two elements of type T.
#define swap(a, b, T)                   \
    do {                                \
        T tmp = b;                      \
        b = a;                          \
        a = tmp;                        \
    } while(0)

// Macro to shorten the gate evaluation process.
#define cancel_dfs                      \
    do {                                \
        g->evaluated = true;            \
        g->critical_path = -1;          \
        return -1;                      \
    } while(0)

#endif