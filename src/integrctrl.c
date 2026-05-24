#include "integrctrl.h"
#include <dirent.h>
#include <unistd.h>

void print_usage(const char *prog_name) {
    fprintf(stderr, "Использование:\n");
    fprintf(stderr, "  %s -s [-r] -f <файл_бд> <директория>\n", prog_name);
    fprintf(stderr, "  %s -c -f <файл_бд> <директория>\n", prog_name);
    exit(EXIT_FAILURE); // Завершаем программу с кодом ошибки
}

void parse_args(int argc, char *argv[], prog_opts *opts) {
    int opt;

    opts->mode = -1;
    opts->recursive = 0;
    opts->db_file = NULL;
    opts->target_dir = NULL;

    while ((opt = getopt(argc, argv, "scrf:")) != -1) {
        switch (opt) {
        case 's':
            opts->mode = 's';
            break;
        case 'c':
            opts->mode = 'c';
            break;
        case 'r':
            opts->recursive = 1;
            break;
        case 'f':
            opts->db_file = optarg;
            break;
        default:
            print_usage(argv[0]);
        }
    }

    if (optind < argc) {
        opts->target_dir = argv[optind];
    }

    if (opts->mode == -1 || opts->db_file == NULL || opts->target_dir == NULL) {
        fprintf(stderr, "Ошибка: пропущены обязательные параметры\n");
        print_usage(argv[0]);
    }
}

void scan_directory(const char *dir_path, int parent_id, int *next_id,
                    vector_t *vec) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        fprintf(stderr, "Ошибка: не удалось открыть директорию %s\n", dir_path);
        return;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) !=
           NULL) { // Читаем объекты внутри папки один за другим
        // Пропускаем служебные ссылки "." и ".."
        // Если их не пропустить, программа зациклится.
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Обрабатываем только папки и файлы
        if (entry->d_type != DT_DIR && entry->d_type != DT_REG) {
            continue;
        }

        // Создаем file_rec через malloc
        file_rec *rec = malloc(sizeof(file_rec));
        if (rec == NULL) {
            fprintf(stderr, "Ошибка выделения памяти для файла %s\n",
                    entry->d_name);
            continue;
        }

        rec->id = *next_id; // Заполняем поля
        (*next_id)++; // счетчик для некс файла

        rec->parent_id = parent_id;
        strncpy(rec->name, entry->d_name, NAME_LEN - 1);
        rec->name[NAME_LEN - 1] = '\0';
        memset(rec->md5, 0, 16); // обнуляем хэш по умолчанию

        if (entry->d_type == DT_REG) { // Обработка файлов и папок
            rec->type = TYPE_FILE;

            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", dir_path,
                     entry->d_name);

            FILE *fp = fopen(full_path, "rb");
            if (fp != NULL) {
                md5File(fp, rec->md5); // Считаем MD5
                fclose(fp);
            } else {
                fprintf(stderr, "Ошибка чтения файла: %s\n", full_path);
            }

        } else if (entry->d_type == DT_DIR) {
            rec->type =
                TYPE_DIR; // Для директорий md5 уже заполнен нулями через memset
        }

        vec_push(vec, rec);
    }

    closedir(dir);
}