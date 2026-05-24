#include "check.h"

static file_rec *find_by_name(vector_t *vec, const char *name) {
    for (size_t i = 0; i < vec_len(vec); i++) {
        file_rec *rec = vec_get(vec, i);
        if (rec->parent_id != 0 && strcmp(rec->name, name) == 0)
            return rec;
    }
    return NULL;
}

static int md5_equal(unsigned char *a, unsigned char *b) {
    for (int i = 0; i < 16; i++)
        if (a[i] != b[i])
            return 0;
    return 1;
}

void check_integrity(vector_t *old_vec, vector_t *new_vec) {
    for (size_t i = 0; i < vec_len(old_vec); i++) {
        file_rec *old = vec_get(old_vec, i);
        if (old->parent_id == 0)
            continue;

        file_rec *cur = find_by_name(new_vec, old->name);
        if (cur == NULL) {
            printf("%-40s MISSING\n", old->name);
        } else if (old->type == TYPE_FILE && !md5_equal(old->md5, cur->md5)) {
            printf("%-40s CHANGED\n", old->name);
        } else {
            printf("%-40s OK\n", old->name);
        }
    }

    for (size_t i = 0; i < vec_len(new_vec); i++) {
        file_rec *cur = vec_get(new_vec, i);
        if (cur->parent_id == 0)
            continue;

        if (find_by_name(old_vec, cur->name) == NULL)
            printf("%-40s NEW\n", cur->name);
    }
}
