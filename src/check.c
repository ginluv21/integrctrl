#include "check.h"

static file_rec *find_by_id(vector_t *vec, int id) {
    for (size_t i = 0; i < vec_len(vec); i++) {
        file_rec *rec = vec_get(vec, i);
        if (rec->id == id)
            return rec;
    }
    return NULL;
}

static void build_path(vector_t *vec, file_rec *rec, char *buf, size_t size) {
    if (rec->parent_id == 0) {
        buf[0] = '\0';
        return;
    }
    file_rec *parent = find_by_id(vec, rec->parent_id);
    if (parent == NULL || parent->parent_id == 0) {
        strncpy(buf, rec->name, size - 1);
        buf[size - 1] = '\0';
        return;
    }
    build_path(vec, parent, buf, size);
    size_t len = strlen(buf);
    if (len + 1 < size) {
        buf[len] = '/';
        buf[len + 1] = '\0';
        strncat(buf, rec->name, size - len - 2);
    }
}

static file_rec *find_by_path(vector_t *vec, const char *path) {
    char buf[2048];
    for (size_t i = 0; i < vec_len(vec); i++) {
        file_rec *rec = vec_get(vec, i);
        if (rec->parent_id == 0)
            continue;
        build_path(vec, rec, buf, sizeof(buf));
        if (strcmp(buf, path) == 0)
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

void check_integrity(vector_t *old_vec, vector_t *new_vec, const char *prefix,
                     int stats[4], int recursive) {
    char old_path[2048];
    char new_path[2048];
    char search[4096];
    int has_prefix = (prefix != NULL && prefix[0] != '\0');
    size_t plen = has_prefix ? strlen(prefix) : 0;

    for (size_t i = 0; i < vec_len(old_vec); i++) {
        file_rec *old = vec_get(old_vec, i);
        if (old->parent_id == 0)
            continue;

        build_path(old_vec, old, old_path, sizeof(old_path));

        if (has_prefix) {
            int match =
                strcmp(old_path, prefix) == 0 ||
                (strncmp(old_path, prefix, plen) == 0 && old_path[plen] == '/');
            if (!match)
                continue;
            if (strcmp(old_path, prefix) == 0)
                continue; // сама подпапка - не выводим
            strncpy(search, old_path + plen + 1, sizeof(search) - 1);
        } else {
            strncpy(search, old_path, sizeof(search) - 1);
        }

        if (!recursive && strchr(search, '/') != NULL)
            continue;

        file_rec *cur = find_by_path(new_vec, search);
        const char *t = (old->type == TYPE_FILE) ? "FILE" : "DIR ";

        const char *status;
        if (cur == NULL) {
            status = "MISSING";
            stats[2]++;
        } else if (old->type == TYPE_FILE && !md5_equal(old->md5, cur->md5)) {
            status = "CHANGED";
            stats[1]++;
        } else {
            status = "OK";
            stats[0]++;
        }
        printf("%-23.23s  [%s]  %-7s\n", search, t, status);
    }

    for (size_t i = 0; i < vec_len(new_vec); i++) {
        file_rec *cur = vec_get(new_vec, i);
        if (cur->parent_id == 0)
            continue;

        build_path(new_vec, cur, new_path, sizeof(new_path));

        if (has_prefix)
            snprintf(search, sizeof(search), "%s/%s", prefix, new_path);
        else
            strncpy(search, new_path, sizeof(search) - 1);

        if (!recursive && strchr(new_path, '/') != NULL)
            continue;

        if (find_by_path(old_vec, search) == NULL) {
            const char *t = (cur->type == TYPE_FILE) ? "FILE" : "DIR ";
            printf("%-23.23s  [%s]  %-7s\n", new_path, t, "NEW");
            stats[3]++;
        }
    }
}
