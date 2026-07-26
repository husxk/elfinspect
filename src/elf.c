#include "elf.h"

#include <stdio.h>
#include <stdlib.h>

static void
elf_clear_tables(elf_t *elf)
{
  if (!elf)
    return;

  if (elf->program_headers)
  {
    elf_program_header_table_destroy(elf->program_headers);
    elf->program_headers = NULL;
  }

  if (elf->section_headers)
  {
    elf_section_header_table_destroy(elf->section_headers);
    elf->section_headers = NULL;
  }
}

bool
elf_create(elf_t **out, file_t *file)
{
  if (!out)
  {
    fprintf(stderr, "elf_create: output pointer is null\n");
    return false;
  }

  if (!file)
  {
    fprintf(stderr, "elf_create: file is null\n");
    return false;
  }

  elf_t *elf = calloc(1, sizeof(*elf));
  if (!elf)
  {
    fprintf(stderr, "elf_create: out of memory\n");
    return false;
  }

  elf->file = file;
  *out = elf;
  return true;
}

void
elf_destroy(elf_t *elf)
{
  if (!elf)
    return;

  elf_clear_tables(elf);
  free(elf);
}

bool
elf_parse(elf_t *elf)
{
  if (!elf)
  {
    fprintf(stderr, "elf_parse: elf is null\n");
    return false;
  }

  if (!elf->file)
  {
    fprintf(stderr, "elf_parse: file is null\n");
    return false;
  }

  elf_clear_tables(elf);

  if (!parse_elf_header(elf->file, &elf->header))
    return false;

  if (!elf_program_header_table_create(elf->header.ph_table_entry_count, &elf->program_headers))
    return false;

  if (!parse_elf_program_headers(elf->file, &elf->header, elf->program_headers))
  {
    elf_clear_tables(elf);
    return false;
  }

  if (!elf_section_header_table_create(elf->header.sh_table_entry_count, &elf->section_headers))
  {
    elf_clear_tables(elf);
    return false;
  }

  if (!parse_elf_section_headers(elf->file, &elf->header, elf->section_headers))
  {
    elf_clear_tables(elf);
    return false;
  }

  return true;
}

const elf_header *
elf_ehdr(const elf_t *elf)
{
  if (!elf)
    return NULL;

  return &elf->header;
}

const elf_program_header_table *
elf_program_headers(const elf_t *elf)
{
  if (!elf)
    return NULL;

  return elf->program_headers;
}

const elf_section_header_table *
elf_section_headers(const elf_t *elf)
{
  if (!elf)
    return NULL;

  return elf->section_headers;
}
