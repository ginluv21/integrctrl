#include "integrctrl.h"
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
