#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "file.h"

typedef struct
{
  char *bytes;
  size_t size;
} elf_string_table;

void elf_string_table_destroy(elf_string_table *table);

bool parse_elf_string_table(file_t *f, uint64_t offset, uint64_t size, elf_string_table **out);

const char *elf_string_table_get(const elf_string_table *table, uint32_t offset);
