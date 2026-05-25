#ifndef DB_H
#define DB_H

#include "integrctrl.h"

#define IC_MAGIC 0x41634463

typedef struct db_header{
    unsigned int magic;
    unsigned int record_count;
    int recursive;
} db_header;

int db_save(const char *path, vector_t *vec, int recursive);
vector_t *db_load(const char *path, int *recursive_out);

#endif
