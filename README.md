# integrctrl

Утилита командной строки на языке Си для Linux.
Проверяет целостность файлов через MD5-хеши (Integrity Control).

Курсовая работа по дисциплине «Программирование», СибГУТИ, вариант 2.5.

## Сборка

```
make
```

Исполняемый файл появится в `build/integrctrl`.

## Использование

### Сохранить состояние директории

```
./build/integrctrl -s -f data/snap.bin src/
```

С рекурсивным обходом вложенных папок:

```
./build/integrctrl -s -r -f data/snap.bin src/
```

### Проверить целостность

Директория читается из базы автоматически:

```
./build/integrctrl -c -f data/snap.bin
```

Проверить конкретную подпапку:

```
./build/integrctrl -c -r -f data/snap.bin src/sub
```

### Результат проверки

```
main.c                [FILE]  OK
config.h              [FILE]  CHANGED
utils.c               [FILE]  MISSING
newfile.c             [FILE]  NEW
```

- `OK` — файл не изменился
- `CHANGED` — MD5-хеш отличается от сохранённого
- `MISSING` — файл был в базе, сейчас не найден
- `NEW` — файл не был в базе, появился после сохранения

## Структура проекта

```
src/
  main.c          - точка входа
  integrctrl.c    - сканирование, run_save, run_check
  check.c         - сравнение векторов
  db.c            - сохранение и загрузка базы
  vector.c        - динамический массив
  md5.c           - реализация MD5
include/
  integrctrl.h    - основные структуры
  check.h         - проверка целостности
  db.h            - работа с базой
  vector.h        - динамический массив
  md5.h           - MD5
Makefile
```

## Источники

Реализация MD5 взята из открытого репозитория:
https://github.com/Zunawe/md5-c
