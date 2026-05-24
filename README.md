# integrctrl

Утилита командной строки на языке Си (C99) для Linux.
Проверяет целостность файлов через MD5-хеши (Integrity Control).

Курсовая работа по дисциплине «Программирование», СибГУТИ, вариант 2.5.

## Запуск

Сохранить хеши файлов в базу:
```
integrctrl -s -f database /home/alex/
```

Сохранить рекурсивно (включая вложенные директории):
```
integrctrl -s -r -f database /home/alex/
```

Проверить целостность:
```
integrctrl -c -f database /home/alex/
```

Результат проверки для каждого файла: `OK`, `CHANGED`, `MISSING` или `NEW`.

## Сборка

```
make
```

Исполняемый файл появится в `build/integrctrl`.

## Структура проекта

```
src/
  main.c         - точка входа
  integrctrl.c   - основной код
  md5.c          - реализация MD5
include/
  integrctrl.h   - структуры данных
  md5.h          - заголовок MD5
Makefile
```

## Источники

Реализация MD5 взята из открытого репозитория:
https://github.com/Zunawe/md5-c
