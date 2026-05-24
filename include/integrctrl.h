#ifndef INTEGRCTRL_H
#define INTEGRCTRL_H

#include "md5.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define NAME_LEN 256

enum { TYPE_DIR, TYPE_FILE };

typedef struct file_rec { // Структура для хранения информации о файлах и папках
    int id; // Уникальный идентификатор записи
    char name[NAME_LEN]; // Имя файла или директории
    int type; // Тип: TYPE_DIR (директория) или TYPE_FILE (файл)
    int parent_id; // Идентификатор родительской директории (0 для корневой)
    unsigned char md5[16]; // MD5-хеш для файлов
} file_rec;

typedef struct { // Структура для хранения параметров программы
    int mode; // Режим работы: s (save) или c (check)
    int recursive; // Флаг рекурсии: 1 (включена) или 0 (выключена)
    char *db_file; // Строка с именем файла базы данных
    char *target_dir; // Строка с именем анализируемой папки
} prog_opts;

void parse_args(
    int argc, char *argv[],
    prog_opts *opts); // Функция для парсинга аргументов командной строки
void print_usage(const char *prog_name); // Функция для вывода справки по
                                         // использованию программы
void scan_directory(const char *dir_path, int parent_id, int *next_id,
                    vector_t *vec, int recursive);

#endif