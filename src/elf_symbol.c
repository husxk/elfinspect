#include "elf_symbol.h"

#include "elf_endian.h"

#include <stdio.h>
#include <stdlib.h>

static size_t
elf_symbol_size_for_class(const elf_header *eh)
{
  if (eh->class == ELF_CLASS_64)
    return 24;

  if (eh->class == ELF_CLASS_32)
    return 16;

  return 0;
}

static bool
elf_symbol_entry_size(const elf_header *eh, const elf_section_header *sym_sh, size_t *out)
{
  size_t canonical;
  size_t entry_size;

  if (!out || !eh || !sym_sh)
    return false;

  canonical = elf_symbol_size_for_class(eh);
  if (canonical == 0)
  {
    fprintf(stderr, "elf_symbol_entry_size: invalid ELF class 0x%02x\n", eh->class);
    return false;
  }

  if (sym_sh->entsize != 0)
    entry_size = (size_t)sym_sh->entsize;
  else
    entry_size = canonical;

  if (entry_size != canonical)
  {
    fprintf(stderr,
            "elf_symbol_entry_size: section entsize %zu does not match ELF class (expected %zu)\n",
            entry_size, canonical);
    return false;
  }

  *out = entry_size;
  return true;
}

static bool
is_symbol_section(const elf_section_header_table *sections, size_t index)
{
  return elf_section_header_is_type(sections, index, ELF_SHT_SYMTAB)
      || elf_section_header_is_type(sections, index, ELF_SHT_DYNSYM);
}

static bool
parse_elf_symbol_entry(file_t *f, elf_header *eh, elf_symbol *out)
{
  uint8_t info;
  uint8_t other;

  if (!out)
    return false;

  if (!elf_read_u32(f, eh, &out->name))
    return false;

  if (eh->class == ELF_CLASS_32)
  {
    if (!elf_read_word(f, eh, &out->value))
      return false;

    if (!elf_read_word(f, eh, &out->size))
      return false;
  }

  if (!elf_read_u8(f, eh, &info) || !elf_read_u8(f, eh, &other))
    return false;

  if (!elf_read_u16(f, eh, &out->shndx))
    return false;

  if (eh->class == ELF_CLASS_64)
  {
    if (!elf_read_word(f, eh, &out->value))
      return false;

    if (!elf_read_word(f, eh, &out->size))
      return false;
  }

  out->bind = (uint8_t)(info >> 4);
  out->type = (uint8_t)(info & 0x0f);
  out->visibility = (uint8_t)(other & 0x3);

  return true;
}

void
elf_symbol_table_destroy(elf_symbol_table *table)
{
  if (!table)
    return;

  free(table->entries);

  if (table->strtab)
    elf_string_table_destroy(table->strtab);

  free(table);
}

void
elf_symbol_table_set_destroy(elf_symbol_table_set *set)
{
  if (!set)
    return;

  for (size_t i = 0; i < set->count; i++)
    elf_symbol_table_destroy(set->tables[i]);

  free(set->tables);
  free(set);
}

static bool
parse_elf_symbol_table(file_t *f, elf_header *eh, const elf_section_header_table *sections,
                       size_t section_index, elf_symbol_table **out)
{
  const elf_section_header *sym_sh;
  const elf_section_header *str_sh;
  size_t entry_size;
  size_t count;
  elf_symbol_table *table;

  if (!out)
  {
    fprintf(stderr, "parse_elf_symbol_table: output pointer is null\n");
    return false;
  }

  *out = NULL;

  if (!elf_section_header_table_at(sections, section_index, &sym_sh))
    return false;

  if (!elf_symbol_entry_size(eh, sym_sh, &entry_size))
  {
    fprintf(stderr, "parse_elf_symbol_table: invalid entry size for section %zu\n", section_index);
    return false;
  }

  if (sym_sh->size % entry_size != 0)
  {
    fprintf(stderr, "parse_elf_symbol_table: section %zu size %llu not multiple of entry size %zu\n",
            section_index, (unsigned long long)sym_sh->size, entry_size);
    return false;
  }

  if (!elf_section_header_table_at(sections, sym_sh->link, &str_sh))
    return false;

  if (str_sh->type != ELF_SHT_STRTAB)
  {
    fprintf(stderr, "parse_elf_symbol_table: section %zu link %u is not STRTAB\n", section_index,
            sym_sh->link);
    return false;
  }

  table = calloc(1, sizeof(*table));
  if (!table)
  {
    fprintf(stderr, "parse_elf_symbol_table: out of memory\n");
    return false;
  }

  table->section_index = section_index;

  if (!parse_elf_string_table(f, str_sh->offset, str_sh->size, &table->strtab))
  {
    elf_symbol_table_destroy(table);
    return false;
  }

  count = (size_t)(sym_sh->size / entry_size);
  table->count = count;

  if (count == 0)
  {
    *out = table;
    return true;
  }

  table->entries = calloc(count, sizeof(*table->entries));
  if (!table->entries)
  {
    fprintf(stderr, "parse_elf_symbol_table: out of memory\n");
    elf_symbol_table_destroy(table);
    return false;
  }

  if (!file_seek(f, sym_sh->offset))
  {
    elf_symbol_table_destroy(table);
    return false;
  }

  for (size_t i = 0; i < count; i++)
  {
    if (!parse_elf_symbol_entry(f, eh, &table->entries[i]))
    {
      fprintf(stderr, "parse_elf_symbol_table: failed to parse symbol %zu in section %zu\n", i,
              section_index);
      elf_symbol_table_destroy(table);
      return false;
    }
  }

  *out = table;
  return true;
}

bool
parse_elf_symbol_tables(file_t *f, elf_header *eh, const elf_section_header_table *sections,
                        elf_symbol_table_set **out)
{
  size_t sym_section_count = 0;
  elf_symbol_table_set *set;

  if (!f)
  {
    fprintf(stderr, "parse_elf_symbol_tables: file is null\n");
    return false;
  }

  if (!eh)
  {
    fprintf(stderr, "parse_elf_symbol_tables: elf header is null\n");
    return false;
  }

  if (!sections)
  {
    fprintf(stderr, "parse_elf_symbol_tables: section headers is null\n");
    return false;
  }

  if (!out)
  {
    fprintf(stderr, "parse_elf_symbol_tables: output pointer is null\n");
    return false;
  }

  *out = NULL;

  for (size_t i = 0; i < sections->count; i++)
  {
    if (is_symbol_section(sections, i))
      sym_section_count++;
  }

  set = calloc(1, sizeof(*set));
  if (!set)
  {
    fprintf(stderr, "parse_elf_symbol_tables: out of memory\n");
    return false;
  }

  if (sym_section_count == 0)
  {
    *out = set;
    return true;
  }

  set->tables = calloc(sym_section_count, sizeof(*set->tables));
  if (!set->tables)
  {
    fprintf(stderr, "parse_elf_symbol_tables: out of memory\n");
    free(set);
    return false;
  }

  for (size_t i = 0; i < sections->count; i++)
  {
    if (!is_symbol_section(sections, i))
      continue;

    if (!parse_elf_symbol_table(f, eh, sections, i, &set->tables[set->count]))
    {
      elf_symbol_table_set_destroy(set);
      return false;
    }

    set->count++;
  }

  *out = set;
  return true;
}
