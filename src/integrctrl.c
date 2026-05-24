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

void scan_directory(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        fprintf(stderr, "Ошибка: не удалось открыть директорию %s\n", dir_path);
        return;
    }

    struct dirent *entry;

    printf("=== Содержание папки %s ===\n", dir_path);

    while ((entry = readdir(dir)) !=
           NULL) { // Читаем объекты внутри папки один за другим
        // Пропускаем служебные ссылки "." и ".."
        // Если их не пропустить, программа зациклится.
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Выводим имя объекта
        printf("Имя: %-20s | ", entry->d_name);

        // Проверяем тип объекта строго по методичке (DT_DIR — папка, DT_REG —
        // файл)
        if (entry->d_type == DT_DIR) {
            printf("Тип: ДИРЕКТОРИЯ\n");
        } else if (entry->d_type == DT_REG) {
            printf("Тип: ОБЫЧНЫЙ ФАЙЛ\n");
        } else {
            printf("Тип: ДРУГОЙ (игнорируем)\n");
        }
    }

    closedir(dir);
}