#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <string.h>

#include <stdio.h>

#include "common/io.h"
#include "common/sumset.h"
#include "common/err.h"

typedef struct Node {
    void* data;
    struct Node* next;
} Node;

typedef struct {
    int tag; // https://en.wikipedia.org/wiki/ABA_problem
    Node* head;
} Stack;

typedef struct Frame {
    Sumset* a;
    Sumset* b;
} Frame;

static atomic_int waiting_computations;
static atomic_bool all_done;

static _Atomic Stack stack = (_Atomic Stack){0, NULL};

static InputData input_data;

static void push(_Atomic Stack* stack, void* data) {
    Stack next, prev;

    Node* new_node = malloc(sizeof(Node));
    if (!new_node) {
        exit(EXIT_FAILURE);
    }
    new_node->data = data;
    
    prev = atomic_load(stack);
    do {
        new_node->next = prev.head;
        next.head = new_node;
        next.tag = prev.tag + 1;
    } while (!atomic_compare_exchange_weak(stack, &prev, next));
}

static void* pop(_Atomic Stack* stack) {
    void* data;
    Stack next, prev;
    
    prev = atomic_load(stack);
    do {
        if (prev.head == NULL) {
            return NULL;
        }
        next.head = prev.head->next;
        next.tag = prev.tag + 1;
    } while(!atomic_compare_exchange_weak(stack, &prev, next));

    data = prev.head->data;
    free(prev.head);

    return data;
}

static bool is_empty(_Atomic Stack* stack) {
    Stack current = atomic_load(stack);
    return current.head == NULL;
}

void* blocking_pop(_Atomic Stack* stack) {
    void* data = NULL;

    while (true) {
        data = pop(stack);
        if (data) {
            return data;
        }

        if (atomic_load(&all_done)) {
            return NULL;
        }

        sched_yield();
    }
}

static Sumset* deep_copy_sumset(const Sumset* original) {
    if (original == NULL) {
        return NULL;
    }

    Sumset* copy = malloc(sizeof(Sumset));
    if (!copy) {
        exit(EXIT_FAILURE);
    }

    memcpy(copy, original, sizeof(Sumset));

    copy->prev = deep_copy_sumset(original->prev);

    return copy;
}

static void deep_free_sumset(Sumset* sumset) {
    while (sumset) {
        const Sumset* next = sumset->prev;
        free((void*)sumset);
        sumset = (Sumset*)next;
    }
}

static Frame* make_frame(const Sumset* a, const Sumset* b) {
    Frame* f = (Frame*)malloc(sizeof(Frame));
    if (!f) {
        exit(EXIT_FAILURE);
    }

    f->a = deep_copy_sumset(a);
    f->b = deep_copy_sumset(b);

    return f;
}

static void recursive_solve(Sumset *a, Sumset *b, Solution *best_solution) {
    if (a->sum > b->sum)
        return recursive_solve(b, a, best_solution);

    if (is_sumset_intersection_trivial(a, b)) { // s(a) ∩ s(b) = {0}.
        for (size_t i = a->last; i <= input_data.d; ++i) {
            if (!does_sumset_contain(b, i)) {
                Sumset a_with_i;
                sumset_add(&a_with_i, a, i);

                if (atomic_load(&waiting_computations) > 0) {
                    Frame* task = make_frame(&a_with_i, b);
                    if (atomic_load(&waiting_computations) > 0) {
                        push(&stack, task);
                        sched_yield();
                    }
                    else {
                        deep_free_sumset(task->a);
                        deep_free_sumset(task->b);
                        free(task);
                        recursive_solve(&a_with_i, b, best_solution);
                    }
                }
                else {
                    recursive_solve(&a_with_i, b, best_solution);
                }
            }
        }
    } else if ((a->sum == b->sum) && (get_sumset_intersection_size(a, b) == 2)) { // s(a) ∩ s(b) = {0, ∑b}.
        if (b->sum > best_solution->sum) {
            solution_build(best_solution, &input_data, a, b);
        }
    }
}

static void* solve(void* arg) {
    Solution *s = (Solution*)(arg);
    solution_init(s);

    while (true) {
        Frame* task = blocking_pop(&stack);

        if (!task) {
            break;
        }

        atomic_fetch_sub(&waiting_computations, 1);
        
        recursive_solve(task->a, task->b, s);

        deep_free_sumset(task->a);
        deep_free_sumset(task->b);
        free(task);

        atomic_fetch_add(&waiting_computations, 1);

        if (is_empty(&stack) && atomic_load(&waiting_computations) == input_data.t) {
            atomic_store(&all_done, true);
        }
    }
    return NULL;
}

int main() {

    input_data_read(&input_data);
    // input_data_init(&input_data, 8, 10, (int[]){0}, (int[]){1, 0});

    Solution best_solution;
    solution_init(&best_solution);
    atomic_store(&waiting_computations, input_data.t);
    atomic_store(&all_done, false);

    push(&stack, make_frame(&input_data.a_start, &input_data.b_start));

    pthread_t *threads = (pthread_t*)malloc(sizeof(pthread_t) * input_data.t);
    if (!threads) {
        exit(EXIT_FAILURE);
    }

    Solution *sols = (Solution*)malloc(sizeof(Solution) * input_data.t);
    if (!sols) {
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < input_data.t; ++i) {
        pthread_create(threads + i, NULL, solve, sols + i);
    }

    for (size_t i = 0; i < input_data.t; ++i) {
        pthread_join(threads[i], NULL);
        if (best_solution.sum < sols[i].sum) {
            best_solution = sols[i];
        }
    }

    free(threads);
    free(sols);
    solution_print(&best_solution);
    return 0;
}