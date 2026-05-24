#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "integrctrl.h"

typedef struct {
    file_rec **data;
    file_rec **res;
    size_t len;
    size_t cap;
} vector_t;

typedef struct {
    vector_t *vec;
    size_t ind;
} vec_iter_t;

vector_t *vec_create(size_t cap);
void vec_destroy(vector_t *vec);
int vec_push(vector_t *vec, file_rec *rec);
file_rec *vec_pop(vector_t *vec);
file_rec *vec_get(vector_t *vec, size_t ind);
size_t vec_len(vector_t *vec);
size_t vec_cap(vector_t *vec);
int vec_insert(vector_t *vec, size_t ind, file_rec *rec);
int vec_remove(vector_t *vec, size_t ind);
int vec_change(vector_t *vec, size_t ind, file_rec *rec);
vector_t *vec_copy(vector_t *vec);
int vec_merge(vector_t *v1, vector_t *v2);
vec_iter_t vec_begin(vector_t *vec);
vec_iter_t vec_end(vector_t *vec);
void vec_iter_next(vec_iter_t *iter);
int vec_isequal(vec_iter_t it1, vec_iter_t it2);
int vec_iter_belong(vec_iter_t it, vector_t *vec);
void print_vector(vector_t *vec);

#endif
