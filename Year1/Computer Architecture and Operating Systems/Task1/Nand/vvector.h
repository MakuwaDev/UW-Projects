#ifndef VVECTOR_H
#define VVECTOR_H

#include <sys/types.h>

typedef struct vvector vvector_t;

vvector_t* vvector_create();
int        vvector_is_empty(const vvector_t *v);
ssize_t    vvector_size(const vvector_t *v);
void*      vvector_get_elem(const vvector_t *v, size_t n);
void*      vvector_back(const vvector_t *v);
int        vvector_pop_back(vvector_t *v);
int        vvector_remove_elem(vvector_t *v, size_t n);
int        vvector_push_back(vvector_t *v, void *val);
ssize_t    vvector_find(const vvector_t *v, void *val);
void       vvector_delete(vvector_t *v);

#endif