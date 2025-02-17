#include "nand.h"
#include "utils.h"
#include "vvector.h"

#include <errno.h>
#include <stdlib.h>

// Structure for storing a single gate input and distinguish its type.
// Type of the input is determined by the value of the 'type' variable:
// type = 'n' <=> object = NULL
// type = 'b' <=> object points to a const bool
// type = 'g' <=> object points to a nand_t
typedef struct in_sig {
  void *object;
  char type;
} in_sig_t;

// A structural type representing a NAND gate.
typedef struct nand {
  // General information about the NAND gate.
  in_sig_t *input_signals;
  vvector_t *output_signals;
  size_t input_count;

  // Auxillary flags used to evaluate the output value of the gate and its
  // critical path.
  ssize_t critical_path;
  bool visited;
  bool evaluated;
  bool value;
} nand_t;

// Auxillary functions used to determine the values of the output signals of
// the given gates and calculate the length of the critical path.
static ssize_t evaluate(nand_t *g);
static ssize_t handle_null(nand_t *g);
static ssize_t handle_bool(nand_t *g, size_t i);
static ssize_t handle_gate(nand_t *g, size_t i);

// Function to handle the case of a NULL at an ith input of gate g while
// evaluating.
static ssize_t handle_null(nand_t *g) {
  cancel_dfs;
}

// Function to handle the case of a bool signal at an ith input of gate g while
// evaluating.
static ssize_t handle_bool(nand_t *g, size_t i) {
  const bool *signal;

  signal = (const bool*)g->input_signals[i].object;
  if (*signal == false)
    g->value = true;
  g->critical_path = max(g->critical_path, 1);

  return g->critical_path;
}

// Function to handle the case of a nand gate at an ith input of gate g while
// evaluating.
static ssize_t handle_gate(nand_t *g, size_t i) {
  nand_t *gate = (nand_t*)g->input_signals[i].object;

  if (gate->visited && !gate->evaluated)
    cancel_dfs;  // Gates form a cycle.
  else {
    // If the gate was not visited we try to evaluate its value
    // and if it was cancelled, we cancel too.
    if (!gate->visited && evaluate(gate) == -1)
      cancel_dfs;

    g->critical_path = max(g->critical_path, gate->critical_path + 1);
    if (gate->value == false)
      g->value = true;
  }

  return g->critical_path;
}

// Recursive function to evaluate the output value of a given nand gate.
static ssize_t evaluate(nand_t *g) {
  // Setting the default values.
  g->visited = true;
  g->value = false;
  g->critical_path = 0;

  // Gate with zero inputs gives the output false.
  if (g->input_count == 0) {
    g->evaluated = true;
    return g->critical_path;
  }

  // Iterating over the inputs of the gate.
  for (size_t i = 0; i < g->input_count; ++i) {
    switch (g->input_signals[i].type) {
      case 'n':
        handle_null(g);
        break;
      case 'b':
        handle_bool(g, i);
        break;
      case 'g':
        handle_gate(g, i);
        break;
    }

    if (g->critical_path == -1)
      return -1;
  }

  g->evaluated = true;
  return g->critical_path;
}

// Recursive function to clean the 'visited' and 'evaluated' values
// previously set by the evaluate function called in gate g.
static void clean(nand_t *g) {
  nand_t *gate;

  g->visited = false;
  g->evaluated = false;

  // Iterating over inputs of the gate and recursively cleaning them
  // if they aren't cleaned up already.
  for (size_t i = 0; i < g->input_count; ++i) {
    if (g->input_signals[i].type == 'g') {
      gate = (nand_t*)g->input_signals[i].object;
      if (gate->visited)
        clean(gate);
    }
  }
}

// Function to find the index of gate g in array t, returns size, if not found.
static ssize_t nand_find(in_sig_t *t, nand_t *g, size_t size) {
  size_t i = 0;

  error_check(t == NULL, EINVAL, -1);

  while (i < size) {
    if (t[i].object == g)
      break;
    ++i;
  }

  return (ssize_t)i;
}

// Function to disconnect the kth input of the given gate.
static void disconnect_input(nand_t *g, size_t k) {
  nand_t *neighbour;
  ssize_t index;

  if (g->input_signals[k].type == 'g') {
    neighbour = (nand_t*)g->input_signals[k].object;
    index = vvector_find(neighbour->output_signals, g);
    vvector_remove_elem(neighbour->output_signals, (size_t)index);
  }

  g->input_signals[k] = (in_sig_t){NULL, 'n'};
}

// Function to disconnect the kth output of the given gate.
static void disconnect_output(nand_t *g, size_t k) {
  nand_t *neighbour;
  ssize_t index;

  neighbour = vvector_get_elem(g->output_signals, k);
  index = nand_find(neighbour->input_signals, g, neighbour->input_count);
  neighbour->input_signals[index] = (in_sig_t){NULL, 'n'};
}

// Function to allocate a new nand gate and return a pointer to it.
nand_t *nand_new(unsigned n) {
  nand_t *res = (nand_t*)malloc(sizeof(nand_t));

  error_check(res == NULL, ENOMEM, NULL);

  // Setting all default values for the new gate
  // and allocating necessary memory.
  res->input_count = n;
  res->evaluated = res->visited = res->value = false;
  res->critical_path = 0;
  res->input_signals = (in_sig_t*)malloc(sizeof(in_sig_t) * (n + 1));
  res->output_signals = vvector_create();

  // Checking whether all allocations ended successfully.
  // Freeing allocated memory if any failed.
  if (res->input_signals == NULL || res->output_signals == NULL) {
    if (res->input_signals != NULL)
      free(res->input_signals);
    if (res->output_signals != NULL)
      vvector_delete(res->output_signals);

    free(res);
    errno = ENOMEM;
    return NULL;
  }

  // Setting more default values.
  for (size_t i = 0; i < n; ++i)
    res->input_signals[i] = (in_sig_t){NULL, 'n'};

  return res;
}

// Function to disconnect the input and output signals of the g gate,
// then delete the gate and free all memory used by it.
// Once the function is executed, the pointer passed to it becomes invalid.
void nand_delete(nand_t *g) {
  if (g == NULL)
    return;

  // Disconnecting input signals of the gate.
  for (size_t i = 0; i < g->input_count; ++i)
    disconnect_input(g, i);

  // Disconnecting output signals of the gate.
  for (ssize_t i = (ssize_t)vvector_size(g->output_signals) - 1; i >= 0; --i) {
    disconnect_output(g, i);
  }

  // Freeing the memory allocated by the gate.
  free(g->input_signals);
  vvector_delete(g->output_signals);
  g->evaluated = false;
  g->visited = false;
  g->input_count = g->critical_path = 0;
  free(g);
}

// Function to connect the output of the g_out gate to the kth input of
// the g_in gate, possibly disconnecting from this input a signal
// that was previously connected to it.
int nand_connect_nand(nand_t *g_out, nand_t *g_in, unsigned k) {
  // Error handling and the first part of connecting the new signal.
  error_check(g_out == NULL || g_in == NULL || k >= g_in->input_count, 
              EINVAL, -1);
  error_check(vvector_push_back(g_out->output_signals, g_in) == -1, ENOMEM, -1);

  // Disconnecting the signal that was previously connected to the input.
  disconnect_input(g_in, k);

  // Second part of connecting the new signal.
  g_in->input_signals[k].object = g_out;
  g_in->input_signals[k].type = 'g';
  return 0;
}

// Function to connect the boolean signal to the kth input of the g gate,
// possibly disconnecting from this input a signal
// that was previously connected to it.
int nand_connect_signal(bool const *s, nand_t *g, unsigned k) {
  error_check(s == NULL || g == NULL || k >= g->input_count, EINVAL, -1);

  // Disconnecting the signal that was previously connected to the input.
  disconnect_input(g, k);

  // Connecting the new signal.
  g->input_signals[k].object = (void*)s;
  g->input_signals[k].type = 'b';
  return 0;
}

// Function to determine the values of the output signals of the given gates
// and calculate the length of the critical path.
ssize_t nand_evaluate(nand_t **g, bool *s, size_t m) {
  ssize_t res = 0;

  // Error handling.
  error_check(g == NULL || s == NULL || m == 0, EINVAL, -1);
  for (size_t i = 0; i < m; ++i)
    error_check(g[i] == NULL, EINVAL, -1);

  // Evaluating.
  for (size_t i = 0; i < m; ++i) {
    if (!g[i]->evaluated)
      evaluate(g[i]);

    // Assigning values.
    s[i] = g[i]->value;
    res = max(res, g[i]->critical_path);

    // Error handling.
    if (g[i]->critical_path == -1) {
      res = -1;
      break;
    }
  }

  // Cleaning.
  for (size_t i = 0; i < m; ++i)
    if (g[i]->visited)
      clean(g[i]);

  if (res == -1)
    errno = ECANCELED;
  return res;
}

// Function to determine the number of gate inputs
// connected to the output of a given gate.
ssize_t nand_fan_out(nand_t const *g) {
  error_check(g == NULL, EINVAL, -1);
  return vvector_size(g->output_signals);
}

// Function to return a pointer to a boolean signal or a gate connected
// to the kth input of the gate pointed to by g,
// or NULL if nothing is connected to that input.
void *nand_input(nand_t const *g, unsigned k) {
  error_check(g == NULL || k >= g->input_count, EINVAL, NULL);
  errno = 0;
  return g->input_signals[k].object;
}

// Function allowing user to iterate through the gates connected to
// the output of the given gate. The result of this function is indeterminate
// if its parameters are invalid. If the output of gate g is connected to
// multiple inputs of the same gate, then that gate appears
// multiple times through iteration.
nand_t *nand_output(nand_t const *g, ssize_t k) {
  error_check(g == NULL || k >= vvector_size(g->output_signals) || k < 0,
              EINVAL, NULL);
  return (nand_t*)vvector_get_elem(g->output_signals, k);
}