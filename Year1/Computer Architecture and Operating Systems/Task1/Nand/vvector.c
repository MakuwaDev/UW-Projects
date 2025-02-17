#include "vvector.h"
#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

// A structural type to represent a vector of void pointers.
// Note that this vector DOES NOT work like a C++ vector.
// Namely it can remove any element in O(1),
// at the cost of not maintaining the order of the elements.
typedef struct vvector {
  size_t size;
  size_t capacity;
  void **array;
} vvector_t;

// Function to allocate a new vector and return a pointer to it.
vvector_t *vvector_create() {
  // Allocating the necessary memory and error handling.
  vvector_t *res = (vvector_t*)malloc(sizeof(vvector_t));
  error_check(res == NULL, ENOMEM, NULL);

  // Setting the default values.
  res->size = 0;
  res->capacity = 0;
  res->array = NULL;

  return res;
}

// Function to determine whether the given vector is empty or not.
int vvector_is_empty(const vvector_t *v) {
  error_check(v == NULL, EINVAL, -1);
  return v->size == 0;
}

// Function to return the size of the given vector.
ssize_t vvector_size(const vvector_t *v) {
  error_check(v == NULL, EINVAL, -1);
  return (ssize_t)v->size;
}

// Function to return the value (void*) of the nth element in the given vector.
void *vvector_get_elem(const vvector_t *v, size_t n) {
  error_check(v == NULL || n >= v->size, EINVAL, NULL);
  return v->array[n];
}

// Function to return the value (void*) of the last element in the given vector.
void *vvector_back(const vvector_t *v) {
  error_check(v == NULL || v->size == 0, EINVAL, NULL);
  return vvector_get_elem(v, v->size - 1);
}

// Function to remove the last element from the given vector.
int vvector_pop_back(vvector_t *v) {
  error_check(v == NULL || v->size == 0, EINVAL, -1);

  v->array[v->size - 1] = NULL;
  --(v->size);

  return 0;
}

// Function to remove the nth element from the given vector.
// Note that this operation disrupts the order of the vector,
// as it works by swapping the nth element with the last one
// and then removing the last element.
int vvector_remove_elem(vvector_t *v, size_t n) {
  error_check(v == NULL || n >= v->size, EINVAL, -1);
  swap(v->array[n], v->array[v->size - 1], void*);
  vvector_pop_back(v);
  return 0;
}

// Function to add a new element to the back of the given vector.
int vvector_push_back(vvector_t *v, void *val) {
  error_check(v == NULL, EINVAL, -1);
  if (v->capacity == 0) {
    // Adding the first element to the vector.
    // Allocating for the first time and error handling.
    v->array = (void**)malloc(sizeof(void*));
    error_check(v->array == NULL, ENOMEM, -1);

    v->size = v->capacity = 1;
    v->array[0] = val;
  } else {
    if (v->size == v->capacity) {
      // Out of allocated space.
      // Reallocating and error handling.
      void **tmp = (void**)realloc(v->array, sizeof(void*) * v->capacity * 2);
      error_check(tmp == NULL, ENOMEM, -1);
      v->array = tmp;
      v->capacity *= 2;
    }
    v->array[v->size] = val;
    ++(v->size);
  }

  return 0;
}

// Function to find the first index equal to val in the given vector.
// Returns size if none is found.
ssize_t vvector_find(const vvector_t *v, void *val) {
  size_t i = 0;

  error_check(v == NULL, EINVAL, -1);

  while (i < v->size) {
    if (v->array[i] == val)
      break;
    ++i;
  }

  return (ssize_t)i;
}

// Function to delete the given vector and free all memory used by it.
// Once the function is executed, the pointer passed to it becomes invalid.
void vvector_delete(vvector_t *v) {
  if (v == NULL)
    return;

  free(v->array);
  v->size = v->capacity = 0;
  free(v);
}