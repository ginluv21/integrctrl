#ifndef CHECK_H
#define CHECK_H

#include "integrctrl.h"

void check_integrity(vector_t *old_vec, vector_t *new_vec, const char *prefix, int stats[4], int recursive);

#endif
