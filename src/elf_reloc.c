#include "elf_reloc.h"

#include "elf_endian.h"

#include <stdio.h>
#include <stdlib.h>

static size_t
elf_reloc_entry_size_for_class(const elf_header *eh, bool is_rela)
{
  if (eh->class == ELF_CLASS_64)
    return is_rela ? 24 : 16;

  if (eh->class == ELF_CLASS_32)
    return is_rela ? 12 : 8;

  return 0;
}

static bool
is_reloc_section(const elf_section_header_table *sections, size_t index, bool *is_rela_out)
{
  if (!elf_section_header_is_type(sections, index, ELF_SHT_REL)
      && !elf_section_header_is_type(sections, index, ELF_SHT_RELA))
    return false;

  *is_rela_out = elf_section_header_is_type(sections, index, ELF_SHT_RELA);
  return true;
}

static bool
elf_reloc_entry_size(const elf_header *eh, const elf_section_header *reloc_sh, bool is_rela,
                       size_t *out)
{
  size_t canonical;

  if (!out || !eh || !reloc_sh)
    return false;

  canonical = elf_reloc_entry_size_for_class(eh, is_rela);
  if (canonical == 0)
  {
    fprintf(stderr, "elf_reloc_entry_size: invalid ELF class 0x%02x\n", eh->class);
    return false;
  }

  if (reloc_sh->entsize != 0 && reloc_sh->entsize != canonical)
  {
    fprintf(stderr,
            "elf_reloc_entry_size: section entsize %llu, expected %zu\n",
            (unsigned long long)reloc_sh->entsize, canonical);
    return false;
  }

  *out = canonical;
  return true;
}

static bool
parse_elf_reloc_entry(file_t *f, elf_header *eh, bool is_rela, elf_reloc_entry *out)
{
  uint64_t info64;
  uint32_t info32;

  if (!out)
    return false;

  if (!elf_read_word(f, eh, &out->offset))
    return false;

  if (eh->class == ELF_CLASS_64)
  {
    if (!elf_read_u64(f, eh, &info64))
      return false;

    out->sym = (uint32_t)(info64 >> 32);
    out->type = (uint32_t)(info64 & 0xffffffffu);
  }
  else if (eh->class == ELF_CLASS_32)
  {
    if (!elf_read_u32(f, eh, &info32))
      return false;

    out->sym = info32 >> 8;
    out->type = info32 & 0xffu;
  }
  else
  {
    fprintf(stderr, "parse_elf_reloc_entry: invalid class 0x%02x\n", eh->class);
    return false;
  }

  if (is_rela)
  {
    uint64_t addend_raw;

    if (!elf_read_word(f, eh, &addend_raw))
      return false;

    if (eh->class == ELF_CLASS_32)
      out->addend = (int32_t)(uint32_t)addend_raw;
    else
      out->addend = (int64_t)addend_raw;
  }
  else
    out->addend = 0;

  return true;
}

void
elf_reloc_table_destroy(elf_reloc_table *table)
{
  if (!table)
    return;

  free(table->entries);
  free(table);
}

void
elf_reloc_table_set_destroy(elf_reloc_table_set *set)
{
  if (!set)
    return;

  for (size_t i = 0; i < set->count; i++)
    elf_reloc_table_destroy(set->tables[i]);

  free(set->tables);
  free(set);
}

static bool
parse_elf_reloc_table(file_t *f, elf_header *eh, const elf_section_header *reloc_sh,
                      size_t section_index, bool is_rela, elf_reloc_table **out)
{
  size_t entry_size;
  size_t count;
  elf_reloc_table *table;

  if (!out)
    return false;

  *out = NULL;

  if (!elf_reloc_entry_size(eh, reloc_sh, is_rela, &entry_size))
    return false;

  if (reloc_sh->size % entry_size != 0)
  {
    fprintf(stderr, "parse_elf_reloc_table: section %zu size not multiple of entry size\n",
            section_index);
    return false;
  }

  table = calloc(1, sizeof(*table));
  if (!table)
    return false;

  table->section_index = section_index;
  table->symtab_section_index = reloc_sh->link;
  table->is_rela = is_rela;
  table->section_offset = reloc_sh->offset;

  count = (size_t)(reloc_sh->size / entry_size);
  table->count = count;

  if (count == 0)
  {
    *out = table;
    return true;
  }

  table->entries = calloc(count, sizeof(*table->entries));
  if (!table->entries)
  {
    elf_reloc_table_destroy(table);
    return false;
  }

  if (!file_seek(f, reloc_sh->offset))
  {
    elf_reloc_table_destroy(table);
    return false;
  }

  for (size_t i = 0; i < count; i++)
  {
    if (!parse_elf_reloc_entry(f, eh, is_rela, &table->entries[i]))
    {
      fprintf(stderr, "parse_elf_reloc_table: failed to parse entry %zu in section %zu\n", i,
              section_index);
      elf_reloc_table_destroy(table);
      return false;
    }
  }

  *out = table;
  return true;
}

bool
parse_elf_reloc_tables(file_t *f, elf_header *eh, const elf_section_header_table *sections,
                       elf_reloc_table_set **out)
{
  size_t reloc_section_count = 0;
  elf_reloc_table_set *set;

  if (!f || !eh || !sections || !out)
  {
    fprintf(stderr, "parse_elf_reloc_tables: invalid argument\n");
    return false;
  }

  *out = NULL;

  for (size_t i = 0; i < sections->count; i++)
  {
    bool is_rela;

    if (is_reloc_section(sections, i, &is_rela))
      reloc_section_count++;
  }

  set = calloc(1, sizeof(*set));
  if (!set)
    return false;

  if (reloc_section_count == 0)
  {
    *out = set;
    return true;
  }

  set->tables = calloc(reloc_section_count, sizeof(*set->tables));
  if (!set->tables)
  {
    free(set);
    return false;
  }

  for (size_t i = 0; i < sections->count; i++)
  {
    const elf_section_header *reloc_sh;
    bool is_rela;

    if (!is_reloc_section(sections, i, &is_rela))
      continue;

    if (!elf_section_header_table_at(sections, i, &reloc_sh))
    {
      elf_reloc_table_set_destroy(set);
      return false;
    }

    if (!parse_elf_reloc_table(f, eh, reloc_sh, i, is_rela, &set->tables[set->count]))
    {
      elf_reloc_table_set_destroy(set);
      return false;
    }

    set->count++;
  }

  *out = set;
  return true;
}
