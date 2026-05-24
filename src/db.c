#include "db.h"

int db_save(const char *path, vector_t *vec, int recursive) {
    if (path == NULL || vec == NULL)
        return 1;

    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        fprintf(stderr, "Ошибка: не удалось открыть файл %s для записи\n",
                path);
        return 1;
    }

    db_header hdr;
    hdr.magic = IC_MAGIC;
    hdr.record_count = (unsigned int)vec_len(vec);
    hdr.recursive = recursive;

    fwrite(&hdr, sizeof(db_header), 1, f);

    for (size_t i = 0; i < vec_len(vec); i++) {
        file_rec *rec = vec_get(vec, i);
        fwrite(rec, sizeof(file_rec), 1, f);
    }

    fclose(f);
    return 0;
}

vector_t *db_load(const char *path, int *recursive_out) {
    if (path == NULL)
        return NULL;

    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        fprintf(stderr, "Ошибка: не удалось открыть файл %s для чтения\n",
                path);
        return NULL;
    }

    db_header hdr;
    if (fread(&hdr, sizeof(db_header), 1, f) != 1) {
        fprintf(stderr, "Ошибка: не удалось прочитать заголовок базы\n");
        fclose(f);
        return NULL;
    }

    if (hdr.magic != IC_MAGIC) {
        fprintf(stderr,
                "Ошибка: неверный формат файла (неправильный magic number)\n");
        fclose(f);
        return NULL;
    }

    if (recursive_out != NULL)
        *recursive_out = hdr.recursive;

    vector_t *vec = vec_create(hdr.record_count);
    if (vec == NULL) {
        fclose(f);
        return NULL;
    }

    for (unsigned int i = 0; i < hdr.record_count; i++) {
        file_rec *rec = malloc(sizeof(file_rec));
        if (rec == NULL) {
            fprintf(stderr, "Ошибка выделения памяти\n");
            vec_destroy(vec);
            fclose(f);
            return NULL;
        }
        if (fread(rec, sizeof(file_rec), 1, f) != 1) {
            fprintf(stderr, "Ошибка: файл базы повреждён или обрезан\n");
            free(rec);
            vec_destroy(vec);
            fclose(f);
            return NULL;
        }
        vec_push(vec, rec);
    }

    fclose(f);
    return vec;
}
