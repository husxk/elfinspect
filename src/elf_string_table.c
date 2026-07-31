#include "elf_string_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
elf_string_table_destroy(elf_string_table *table)
{
  if (!table)
    return;

  free(table->bytes);
  free(table);
}

bool
parse_elf_string_table(file_t *f, uint64_t offset, uint64_t size, elf_string_table **out)
{
  if (!f)
  {
    fprintf(stderr, "parse_elf_string_table: file is null\n");
    return false;
  }

  if (!out)
  {
    fprintf(stderr, "parse_elf_string_table: output pointer is null\n");
    return false;
  }

  *out = NULL;

  if (size == 0)
  {
    fprintf(stderr, "parse_elf_string_table: size is zero\n");
    return false;
  }

  if (size > SIZE_MAX)
  {
    fprintf(stderr, "parse_elf_string_table: size %llu exceeds address space\n",
            (unsigned long long)size);
    return false;
  }

  if (offset > UINT64_MAX - size)
  {
    fprintf(stderr, "parse_elf_string_table: offset + size overflows\n");
    return false;
  }

  elf_string_table *table = calloc(1, sizeof(*table));
  if (!table)
  {
    fprintf(stderr, "parse_elf_string_table: out of memory\n");
    return false;
  }

  table->size = (size_t)size;
  table->bytes = malloc(table->size + 1);
  if (!table->bytes)
  {
    fprintf(stderr, "parse_elf_string_table: out of memory\n");
    free(table);
    return false;
  }

  if (!file_seek(f, offset))
  {
    elf_string_table_destroy(table);
    return false;
  }

  int n = file_read(f, table->bytes, table->size, table->size);
  if (n < 0 || (size_t)n != table->size)
  {
    fprintf(stderr, "parse_elf_string_table: expected %zu bytes at offset %llu, read %d\n",
            table->size, (unsigned long long)offset, n);
    elf_string_table_destroy(table);
    return false;
  }

  table->bytes[table->size] = '\0';

  if (table->bytes[0] != '\0')
  {
    fprintf(stderr, "parse_elf_string_table: first byte is not NUL (invalid string table)\n");
    elf_string_table_destroy(table);
    return false;
  }

  *out = table;
  return true;
}

const char *
elf_string_table_get(const elf_string_table *table, uint32_t offset)
{
  if (!table || !table->bytes)
    return NULL;

  if ((size_t)offset >= table->size)
    return NULL;

  for (size_t i = (size_t)offset; i < table->size; i++)
  {
    if (table->bytes[i] == '\0')
      return table->bytes + offset;
  }

  return NULL;
}
