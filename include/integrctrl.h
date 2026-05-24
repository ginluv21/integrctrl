#ifndef INTEGRCTRL_H
#define INTEGRCTRL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> // для ssize_t
#include "md5.h"

#define NAME_LEN 256

#define NAME_LEN 256

enum {
    TYPE_DIR,
    TYPE_FILE
};

typedef struct file_rec {
    int id;
    char name[NAME_LEN];
    int type;
    int parent_id;
    unsigned char md5[16];
};

#endif