#pragma once

#include <stdbool.h>

#include "elf_file_header.h"
#include "elf_program_header.h"
#include "elf_section_header.h"
#include "elf_string_table.h"
#include "elf_symbol.h"
#include "elf_dynamic.h"
#include "elf_note.h"
#include "elf_interp.h"
#include "file.h"

typedef struct elf elf_t;

struct elf
{
  file_t *file;
  elf_header header;
  elf_program_header_table *program_headers;
  elf_section_header_table *section_headers;
  elf_string_table *shstrtab;
  elf_symbol_table_set *symbol_tables;
  elf_dynamic *dynamic;
  elf_note_table_set *notes;
  char *interp;
};

bool elf_create(elf_t **out, file_t *file);
void elf_destroy(elf_t *elf);
bool elf_parse(elf_t *elf);

const elf_header *elf_ehdr(const elf_t *elf);
const elf_program_header_table *elf_program_headers(const elf_t *elf);
const elf_section_header_table *elf_section_headers(const elf_t *elf);
const elf_string_table *elf_shstrtab(const elf_t *elf);
const elf_symbol_table_set *elf_symbol_tables(const elf_t *elf);
const elf_dynamic *elf_dynamic_section(const elf_t *elf);
const elf_note_table_set *elf_notes(const elf_t *elf);
const char *elf_interp(const elf_t *elf);
