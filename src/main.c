#include "db.h"

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
        scan_directory(opts.target_dir, root_rec->id, &next_id, vec);

        // Выводим результат
        print_vector(vec);

        // Сохраняем базу в файл
        if (db_save(opts.db_file, vec, opts.recursive) == 0)
            printf("[Сохранение] База записана в %s\n", opts.db_file);
        else
            fprintf(stderr, "[Сохранение] Не удалось записать базу\n");

        vec_destroy(vec);

    } else {
        printf("\n[Проверка] Начинаем сверку с базой %s...\n", opts.db_file);
    }

    return 0;
}
