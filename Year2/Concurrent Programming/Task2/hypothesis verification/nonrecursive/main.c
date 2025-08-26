#include <stddef.h>
#include <stdbool.h>

#include <stdio.h>

#include "common/io.h"
#include "common/sumset.h"

#define MAX_STACK_SIZE MAX_D * MAX_D + 1

typedef struct Frame {
    Sumset a;
    Sumset* b;
    int last;
    bool intersect;
} Frame;

static InputData input_data;
static Solution best_solution;

static Frame stack[MAX_STACK_SIZE];
static size_t stack_size = 0;

inline void swap(Sumset** a, Sumset** b) {
    Sumset* tmp = *a;
    *a = *b;
    *b = tmp;
}

int main() {
    input_data_read(&input_data);
    //input_data_init(&input_data, 1, 16, (int[]){0}, (int[]){1, 0});

    solution_init(&best_solution);

    Sumset firstA = input_data.a_start, firstB = input_data.b_start, helper;
    Frame* current;
    Sumset* a;
    Sumset* b;
    bool pushed;
    size_t i;

    stack[stack_size++] = (Frame){firstA, &firstB, 1, is_sumset_intersection_trivial(&firstA, &firstB)};

    while (stack_size) {
        current = &stack[stack_size - 1];
        a = &current->a;
        b = current->b;

        if (a->sum > b->sum) {
            swap(&a, &b);
        }

        if (current->intersect) {
            pushed = false;
            for (i = current->last; i <= input_data.d; ++i) {
                if (!does_sumset_contain(b, i)) {
                    stack[stack_size++] = (Frame){helper, b, a->sum + i <= b->sum ? i : b->last, false};
                    sumset_add(&(stack[stack_size - 1].a), a, i);
                    stack[stack_size - 1].intersect = is_sumset_intersection_trivial(&(stack[stack_size - 1].a), b);
                    current->last = i + 1;
                    pushed = true;
                    break;
                }
            }
            
            if (!pushed) {
                --stack_size;
            }
        } else {
            if ((a->sum == b->sum) && 
                (b->sum > best_solution.sum) && 
                (get_sumset_intersection_size(a, b) == 2)) {
                        solution_build(&best_solution, &input_data, a, b);
                    }
            --stack_size;
        } 
    }
    
    solution_print(&best_solution);
    return 0;
}