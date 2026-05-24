#include "check.h"
#include "db.h"
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    prog_opts opts;
    parse_args(argc, argv, &opts);

    if (opts.mode == 's') {
        printf("\n[Сохранение] Начинаем обход директории %s...\n",
               opts.target_dir);

        // Создаем вектор
        vector_t *vec = vec_create(10);
        int next_id = 1;

        // Создаем корневую запись (parent_id = 0)
        file_rec *root_rec = malloc(sizeof(file_rec));
        root_rec->id = next_id++;
        root_rec->parent_id = 0;
        root_rec->type = TYPE_DIR;
        strncpy(root_rec->name, opts.target_dir, NAME_LEN - 1);
        memset(root_rec->md5, 0, 16);
        vec_push(vec, root_rec);

        // Сканируем папку (передаем id корневой записи как parent_id)
        scan_directory(opts.target_dir, root_rec->id, &next_id, vec, opts.recursive);

        // Выводим результат
        print_vector(vec);

        // Создаем папку для базы если её нет
        char dir_buf[1024];
        strncpy(dir_buf, opts.db_file, sizeof(dir_buf) - 1);
        char *slash = strrchr(dir_buf, '/');
        if (slash != NULL) {
            *slash = '\0';
            mkdir(dir_buf, 0755);
        }

        // Сохраняем базу в файл
        if (db_save(opts.db_file, vec, opts.recursive) == 0)
            printf("[Сохранение] База записана в %s\n", opts.db_file);
        else
            fprintf(stderr, "[Сохранение] Не удалось записать базу\n");

        vec_destroy(vec);

    } else {
        printf("\n[Проверка] Начинаем сверку с базой %s...\n", opts.db_file);

        int db_recursive = 0;
        vector_t *old_vec = db_load(opts.db_file, &db_recursive);
        if (old_vec == NULL) {
            fprintf(stderr, "Ошибка: не удалось загрузить базу %s\n", opts.db_file);
            return 1;
        }

        vector_t *new_vec = vec_create(10);
        int next_id = 1;

        file_rec *root_rec = malloc(sizeof(file_rec));
        root_rec->id = next_id++;
        root_rec->parent_id = 0;
        root_rec->type = TYPE_DIR;
        strncpy(root_rec->name, opts.target_dir, NAME_LEN - 1);
        memset(root_rec->md5, 0, 16);
        vec_push(new_vec, root_rec);

        scan_directory(opts.target_dir, root_rec->id, &next_id, new_vec, db_recursive);

        printf("\n");
        check_integrity(old_vec, new_vec);

        vec_destroy(old_vec);
        vec_destroy(new_vec);
    }

    return 0;
}
