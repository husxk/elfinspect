#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "elf_file_header.h"
#include "elf_section_header.h"
#include "file.h"

#define ELF_NT_GNU_ABI_TAG           1
#define ELF_NT_GNU_BUILD_ID           3
#define ELF_NT_GNU_PROPERTY_TYPE_0    5

typedef struct
{
  char *name;
  uint32_t type;
  uint8_t *desc;
  size_t descsz;
} elf_note_entry;

typedef struct
{
  size_t section_index;
  elf_note_entry *entries;
  size_t count;
} elf_note_table;

typedef struct
{
  elf_note_table **tables;
  size_t count;
} elf_note_table_set;

void elf_note_entry_destroy(elf_note_entry *entry);
void elf_note_table_destroy(elf_note_table *table);
void elf_note_table_set_destroy(elf_note_table_set *set);

bool parse_elf_note_tables(file_t *f, elf_header *eh, const elf_section_header_table *sections,
                           elf_note_table_set **out);
