#ifndef DB_H
#define DB_H

#include "integrctrl.h"

#define IC_MAGIC 0x41634463

typedef struct { // Структура заголовка базы данных
    unsigned int magic; // Магический номер базы данных "AcDc"
    unsigned int record_count; // Количество записей в базе
    int recursive; // Флаг рекурсии
} db_header;

int db_save(const char *path, vector_t *vec,
            int recursive); // Сохранить базу в файл
vector_t *db_load(const char *path,
                  int *recursive_out); // Загрузить базу из файла

#endif
