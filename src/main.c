#include "integrctrl.h"

int main(int argc, char *argv[]) {
    prog_opts opts;

    parse_args(argc, argv, &opts);

    printf("===Проверка разбора через структуру===\n");
    printf("Режим работы: %c\n", opts.mode);
    printf("Рекурсия: %d\n", opts.recursive);
    printf("Файл базы данных: %s\n", opts.db_file);
    printf("Папка для анализа: %s\n", opts.target_dir);

    if(opts.mode == 's'){
        printf("\n[Сохранение] Начинаем обход директории %s...\n", opts.target_dir);
    } else {
        printf("\n[Проверка] Начинаем сверку с базой %s...\n", opts.db_file);
    }

    return 0;
}
