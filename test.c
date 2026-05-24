#include "db.h"
#include <sys/stat.h>
#include <unistd.h>

#define TMP_DIR  "/tmp/ic_test"
#define TMP_DB   "/tmp/ic_test.db"

static int passed = 0;
static int failed = 0;

static void check(const char *name, int ok) {
    if (ok) {
        printf("PASS  %s\n", name);
        passed++;
    } else {
        printf("FAIL  %s\n", name);
        failed++;
    }
}

static void make_file(const char *path, const char *content) {
    FILE *f = fopen(path, "w");
    if (f == NULL) return;
    fprintf(f, "%s", content);
    fclose(f);
}

int main(void) {
    printf("=== integrctrl tests ===\n\n");

    // Создаем временную папку и файлы
    mkdir(TMP_DIR, 0755);
    make_file(TMP_DIR "/a.txt", "hello");
    make_file(TMP_DIR "/b.txt", "world");
    make_file(TMP_DIR "/c.txt", "AcDc");

    // --- TEST 1: vec_create/destroy ---
    vector_t *vec = vec_create(4);
    check("vec_create не NULL", vec != NULL);
    check("vec_len == 0 после создания", vec_len(vec) == 0);
    check("vec_cap == 4", vec_cap(vec) == 4);
    vec_destroy(vec);

    // --- TEST 2: scan_directory ---
    vec = vec_create(10);
    int next_id = 1;

    file_rec *root = malloc(sizeof(file_rec));
    root->id = next_id++;
    root->parent_id = 0;
    root->type = TYPE_DIR;
    strncpy(root->name, TMP_DIR, NAME_LEN - 1);
    memset(root->md5, 0, 16);
    vec_push(vec, root);

    scan_directory(TMP_DIR, root->id, &next_id, vec);

    check("scan нашел 4 записи (1 папка + 3 файла)", vec_len(vec) == 4);

    // --- TEST 3: db_save ---
    int save_ok = db_save(TMP_DB, vec, 0);
    check("db_save вернул 0", save_ok == 0);

    FILE *f = fopen(TMP_DB, "rb");
    check("файл базы существует", f != NULL);
    if (f) fclose(f);

    // --- TEST 4: db_load ---
    int recursive_flag = -1;
    vector_t *loaded = db_load(TMP_DB, &recursive_flag);
    check("db_load вернул не NULL", loaded != NULL);
    check("загружено столько же записей", vec_len(loaded) == vec_len(vec));
    check("recursive_flag == 0", recursive_flag == 0);

    // --- TEST 5: сравнение первой записи ---
    file_rec *orig   = vec_get(vec, 0);
    file_rec *from_db = vec_get(loaded, 0);
    check("id первой записи совпадает", orig->id == from_db->id);
    check("name первой записи совпадает", strcmp(orig->name, from_db->name) == 0);
    check("type первой записи совпадает", orig->type == from_db->type);

    // --- TEST 6: magic number - испорченный файл ---
    FILE *bad = fopen("/tmp/bad.db", "wb");
    db_header bad_hdr;
    bad_hdr.magic = 0xDEADBEEF;
    bad_hdr.record_count = 0;
    bad_hdr.recursive = 0;
    fwrite(&bad_hdr, sizeof(db_header), 1, bad);
    fclose(bad);
    vector_t *bad_load = db_load("/tmp/bad.db", NULL);
    check("db_load отклонил неверный magic", bad_load == NULL);

    // Очистка
    vec_destroy(vec);
    if (loaded) vec_destroy(loaded);
    remove(TMP_DB);
    remove("/tmp/bad.db");
    remove(TMP_DIR "/a.txt");
    remove(TMP_DIR "/b.txt");
    remove(TMP_DIR "/c.txt");
    rmdir(TMP_DIR);

    printf("\n%d passed, %d failed\n", passed, failed);
    return failed > 0 ? 1 : 0;
}
