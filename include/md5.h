/* Источник: https://github.com/Zunawe/md5-c */
#ifndef MD5_H
#define MD5_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint64_t size; // Размер входных данных в байтах
    uint32_t buffer[4]; // Буфер для хранения промежуточного результата
    uint8_t input[64]; // Буфер для хранения входных данных которые еще не были
                       // обработаны
    uint8_t digest[16]; // Буфер для хранения результата (MD5-хеша)
} MD5Context;

void md5Init(MD5Context *ctx); // инициализация контекста
void md5Update(MD5Context *ctx, uint8_t *input,
               size_t input_len); // добавление данных в контекст
void md5Finalize(MD5Context *ctx); // завершение алгоритма и получение
                                   // результата в ctx->digest
void md5Step(uint32_t *buffer,
             uint32_t *input); // шаг алгоритма на 512 битах входных данных

void md5String(char *input,
               uint8_t *result); // Получить MD5 от строки result должен быть
                                 // способен хранить 16 байт
void md5File(FILE *file,
             uint8_t *result); // Получить MD5 от файла result должен быть
                               // способен хранить 16 байт

#endif
