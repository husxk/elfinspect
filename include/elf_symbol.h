#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "elf_file_header.h"
#include "elf_section_header.h"
#include "elf_string_table.h"
#include "file.h"

#define ELF_STB_LOCAL  0
#define ELF_STB_GLOBAL 1
#define ELF_STB_WEAK   2

#define ELF_STT_NOTYPE  0
#define ELF_STT_OBJECT  1
#define ELF_STT_FUNC    2
#define ELF_STT_SECTION 3
#define ELF_STT_FILE    4
#define ELF_STT_COMMON  5
#define ELF_STT_TLS     6

#define ELF_STV_DEFAULT    0
#define ELF_STV_INTERNAL   1
#define ELF_STV_HIDDEN     2
#define ELF_STV_PROTECTED  3

typedef struct
{
  uint32_t name;
  uint64_t value;
  uint64_t size;
  uint8_t bind;
  uint8_t type;
  uint8_t visibility;
  uint16_t shndx;
} elf_symbol;

typedef struct
{
  size_t section_index;
  elf_symbol *entries;
  size_t count;
  elf_string_table *strtab;
} elf_symbol_table;

typedef struct
{
  elf_symbol_table **tables;
  size_t count;
} elf_symbol_table_set;

void elf_symbol_table_destroy(elf_symbol_table *table);
void elf_symbol_table_set_destroy(elf_symbol_table_set *set);

bool parse_elf_symbol_tables(file_t *f, elf_header *eh, const elf_section_header_table *sections,
                             elf_symbol_table_set **out);
