#include "elf.h"

#include <inttypes.h>
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

  if (elf->shstrtab)
  {
    elf_string_table_destroy(elf->shstrtab);
    elf->shstrtab = NULL;
  }

  if (elf->symbol_tables)
  {
    elf_symbol_table_set_destroy(elf->symbol_tables);
    elf->symbol_tables = NULL;
  }

  if (elf->dynamic)
  {
    elf_dynamic_destroy(elf->dynamic);
    elf->dynamic = NULL;
  }
}

static bool
elf_parse_shstrtab(elf_t *elf)
{
  elf_header *eh;
  const elf_section_header *shstrtab_sh;

  if (!elf || !elf->section_headers)
  {
    fprintf(stderr, "elf_parse_shstrtab: section headers not loaded\n");
    return false;
  }

  eh = &elf->header;

  if (eh->sh_table_entry_count == 0)
    return true;

  if (eh->sh_string_table_index >= eh->sh_table_entry_count)
  {
    fprintf(stderr,
            "elf_parse_shstrtab: sh_string_table_index %u out of range (count %u)\n",
            eh->sh_string_table_index, eh->sh_table_entry_count);
    return false;
  }

  shstrtab_sh = &elf->section_headers->entries[eh->sh_string_table_index];

  if (shstrtab_sh->type != ELF_SHT_STRTAB)
  {
    fprintf(stderr,
            "elf_parse_shstrtab: section %u is type 0x%08" PRIx32 ", expected STRTAB\n",
            eh->sh_string_table_index, shstrtab_sh->type);
    return false;
  }

  return parse_elf_string_table(elf->file, shstrtab_sh->offset, shstrtab_sh->size, &elf->shstrtab);
}

static bool
elf_parse_symbol_tables(elf_t *elf)
{
  if (!elf || !elf->section_headers)
  {
    fprintf(stderr, "elf_parse_symbol_tables: section headers not loaded\n");
    return false;
  }

  return parse_elf_symbol_tables(elf->file, &elf->header, elf->section_headers, &elf->symbol_tables);
}

static bool
elf_parse_dynamic(elf_t *elf)
{
  if (!elf || !elf->section_headers)
  {
    fprintf(stderr, "elf_parse_dynamic: section headers not loaded\n");
    return false;
  }

  return parse_elf_dynamic(elf->file, &elf->header, elf->section_headers, &elf->dynamic);
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

  if (!elf_parse_shstrtab(elf))
  {
    elf_clear_tables(elf);
    return false;
  }

  if (!elf_parse_symbol_tables(elf))
  {
    elf_clear_tables(elf);
    return false;
  }

  if (!elf_parse_dynamic(elf))
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

const elf_string_table *
elf_shstrtab(const elf_t *elf)
{
  if (!elf)
    return NULL;

  return elf->shstrtab;
}

const elf_symbol_table_set *
elf_symbol_tables(const elf_t *elf)
{
  if (!elf)
    return NULL;

  return elf->symbol_tables;
}

const elf_dynamic *
elf_dynamic_section(const elf_t *elf)
{
  if (!elf)
    return NULL;

  return elf->dynamic;
}
