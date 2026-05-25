#include "check.h"
#include "db.h"
#include "integrctrl.h"
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

void print_usage(const char *prog_name) {
    fprintf(stderr, "Использование:\n");
    fprintf(stderr, "  %s -s [-r] -f <файл_бд> <директория>\n", prog_name);
    fprintf(stderr, "  %s -c -f <файл_бд>\n", prog_name);
    exit(EXIT_FAILURE);
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

    if (opts->mode == -1 || opts->db_file == NULL) {
        fprintf(stderr, "Ошибка: пропущены обязательные параметры\n");
        print_usage(argv[0]);
    }

    if (opts->mode == 's' && opts->target_dir == NULL) {
        fprintf(stderr, "Ошибка: для режима -s нужно указать директорию\n");
        print_usage(argv[0]);
    }
}

void scan_directory(const char *dir_path, int parent_id, int *next_id,
                    vector_t *vec, int recursive) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        fprintf(stderr, "Ошибка: не удалось открыть директорию %s\n", dir_path);
        return;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) !=
           NULL) { // Читаем объекты внутри папки один за другим
        // Пропускаем служебные ссылки . и ..
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Обрабатываем только папки и файлы
        if (entry->d_type != DT_DIR && entry->d_type != DT_REG) {
            continue;
        }

        // Создаем file_rec 
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
            rec->type = TYPE_DIR;
        }

        vec_push(vec, rec);

        if (entry->d_type == DT_DIR && recursive) {
            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", dir_path,
                     entry->d_name);
            scan_directory(full_path, rec->id, next_id, vec, recursive);
        }
    }

    closedir(dir);
}

void run_save(prog_opts *opts) {
    printf("\n[Сохранение] Начинаем обход директории %s...\n", opts->target_dir);

    vector_t *vec = vec_create(10);
    int next_id = 1;

    file_rec *root_rec = malloc(sizeof(file_rec));
    root_rec->id = next_id++;
    root_rec->parent_id = 0;
    root_rec->type = TYPE_DIR;
    strncpy(root_rec->name, opts->target_dir, NAME_LEN - 1);
    memset(root_rec->md5, 0, 16);
    vec_push(vec, root_rec);

    scan_directory(opts->target_dir, root_rec->id, &next_id, vec, opts->recursive);

    // print_vector(vec);

    char dir_buf[1024];
    strncpy(dir_buf, opts->db_file, sizeof(dir_buf) - 1);
    char *slash = strrchr(dir_buf, '/');
    if (slash != NULL) {
        *slash = '\0';
        mkdir(dir_buf, 0755);
    }

    if (db_save(opts->db_file, vec, opts->recursive) == 0)
        printf("[Сохранение] База записана в %s (%zu объектов)\n", opts->db_file, vec_len(vec));
    else
        fprintf(stderr, "[Сохранение] Не удалось записать базу\n");

    vec_destroy(vec);
}

void run_check(prog_opts *opts) {
    printf("\n[Проверка] Начинаем сверку с базой %s...\n", opts->db_file);

    int db_recursive = 0;
    time_t saved_at = 0;
    vector_t *old_vec = db_load(opts->db_file, &db_recursive, &saved_at);
    if (old_vec == NULL) {
        fprintf(stderr, "Ошибка: не удалось загрузить базу %s\n", opts->db_file);
        return;
    }

    // Читаем папку из базы
    file_rec *db_root_rec = vec_get(old_vec, 0);
    const char *db_root = db_root_rec->name;

    // передал папку иначе из базы
    const char *target_dir = (opts->target_dir != NULL) ? opts->target_dir : db_root;

    // Вычисляем отн путь от db_root до target_dir
    char prefix[1024] = "";
    if (opts->target_dir != NULL && strcmp(opts->target_dir, db_root) != 0) {
        char root_buf[1024], tgt_buf[1024];
        strncpy(root_buf, db_root, sizeof(root_buf) - 1);
        strncpy(tgt_buf, opts->target_dir, sizeof(tgt_buf) - 1);
        // убираем trailing slash
        size_t rlen = strlen(root_buf);
        if (rlen > 0 && root_buf[rlen - 1] == '/') root_buf[rlen - 1] = '\0';
        size_t tlen = strlen(tgt_buf);
        if (tlen > 0 && tgt_buf[tlen - 1] == '/') tgt_buf[tlen - 1] = '\0';

        size_t root_len = strlen(root_buf);
        if (strncmp(tgt_buf, root_buf, root_len) == 0 && tgt_buf[root_len] == '/')
            strncpy(prefix, tgt_buf + root_len + 1, sizeof(prefix) - 1);
        else {
            char *sl = strrchr(tgt_buf, '/');
            strncpy(prefix, sl ? sl + 1 : tgt_buf, sizeof(prefix) - 1);
        }
    }

    char time_buf[64];
    struct tm *tm_info = localtime(&saved_at);
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("[Проверка] База сохранена: %s\n", time_buf);
    printf("[Проверка] Директория: %s\n", target_dir);

    vector_t *new_vec = vec_create(10);
    int next_id = 1;

    file_rec *root_rec = malloc(sizeof(file_rec));
    root_rec->id = next_id++;
    root_rec->parent_id = 0;
    root_rec->type = TYPE_DIR;
    strncpy(root_rec->name, target_dir, NAME_LEN - 1);
    memset(root_rec->md5, 0, 16);
    vec_push(new_vec, root_rec);

    // Рекурсия берем из аргументов если не указана из базы
    int recursive = opts->recursive;
    scan_directory(target_dir, root_rec->id, &next_id, new_vec, recursive);

    printf("\n----------------------------------------\n");
    int stats[4] = {0, 0, 0, 0}; // OK, CHANGED, MISSING, NEW
    check_integrity(old_vec, new_vec, prefix[0] ? prefix : NULL, stats, recursive);
    printf("----------------------------------------\n");
    printf("OK: %d  CHANGED: %d  MISSING: %d  NEW: %d\n",
           stats[0], stats[1], stats[2], stats[3]);
    printf("[Проверка] Объектов в базе: %zu, в текущем скане: %zu\n", vec_len(old_vec), vec_len(new_vec));

    vec_destroy(old_vec);
    vec_destroy(new_vec);
}