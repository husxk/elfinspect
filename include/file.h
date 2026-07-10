#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct file file_t;

bool file_open(file_t **out, const char *path);
bool file_close(file_t *f);
int file_read(file_t *f, void *buf, size_t buf_size, size_t count);
