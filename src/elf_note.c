#include "elf_note.h"

#include "elf_endian.h"

#include <endian.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t
note_u32(const elf_header *eh, const uint8_t *raw)
{
  uint32_t v;

  memcpy(&v, raw, sizeof(v));

  if (eh->endian == ELF_ENDIAN_LITTLE)
    return le32toh(v);

  return be32toh(v);
}

static size_t
note_align4(size_t n)
{
  return (n + 3) & ~(size_t)3;
}

void
elf_note_entry_destroy(elf_note_entry *entry)
{
  if (!entry)
    return;

  free(entry->name);
  free(entry->desc);
}

void
elf_note_table_destroy(elf_note_table *table)
{
  if (!table)
    return;

  for (size_t i = 0; i < table->count; i++)
  {
    elf_note_entry_destroy(&table->entries[i]);
  }

  free(table->entries);
  free(table);
}

void
elf_note_table_set_destroy(elf_note_table_set *set)
{
  if (!set)
    return;

  for (size_t i = 0; i < set->count; i++)
    elf_note_table_destroy(set->tables[i]);

  free(set->tables);
  free(set);
}

static bool
parse_elf_notes_from_buffer(const elf_header *eh, const uint8_t *data, size_t size,
                            elf_note_table *table)
{
  size_t offset = 0;
  size_t entry_count = 0;

  while (offset + 12 <= size)
  {
    uint32_t namesz = note_u32(eh, data + offset);
    uint32_t descsz = note_u32(eh, data + offset + 4);

    size_t namesz_pad = note_align4(namesz);
    size_t descsz_pad = note_align4(descsz);

    size_t entry_size = 12 + namesz_pad + descsz_pad;

    if (offset + entry_size > size)
    {
      fprintf(stderr, "parse_elf_notes: note at offset %zu extends past section end\n", offset);
      return false;
    }

    entry_count++;
    offset += entry_size;
  }

  if (entry_count == 0)
    return true;

  table->entries = calloc(entry_count, sizeof(*table->entries));
  if (!table->entries)
  {
    fprintf(stderr, "parse_elf_notes: out of memory\n");
    return false;
  }

  table->count = entry_count;
  offset = 0;

  for (size_t i = 0; i < entry_count; i++)
  {
    elf_note_entry *entry = &table->entries[i];

    uint32_t namesz = note_u32(eh, data + offset);
    uint32_t descsz = note_u32(eh, data + offset + 4);

    size_t namesz_pad = note_align4(namesz);
    size_t descsz_pad = note_align4(descsz);

    const uint8_t *name_raw = data + offset + 12;
    const uint8_t *desc_raw = name_raw + namesz_pad;

    entry->type = note_u32(eh, data + offset + 8);
    entry->descsz = descsz;

    if (namesz > 0)
    {
      entry->name = malloc(namesz + 1);
      if (!entry->name)
      {
        fprintf(stderr, "parse_elf_notes: out of memory\n");
        return false;
      }

      memcpy(entry->name, name_raw, namesz);
      entry->name[namesz] = '\0';
    }

    if (descsz > 0)
    {
      entry->desc = malloc(descsz);
      if (!entry->desc)
      {
        fprintf(stderr, "parse_elf_notes: out of memory\n");
        return false;
      }

      memcpy(entry->desc, desc_raw, descsz);
    }

    offset += 12 + namesz_pad + descsz_pad;
  }

  return true;
}

static bool
parse_elf_note_table(file_t *f, elf_header *eh, const elf_section_header *note_sh,
                     size_t section_index, elf_note_table **out)
{
  uint8_t *data;
  elf_note_table *table;

  if (!out)
    return false;

  *out = NULL;

  if (note_sh->size == 0)
  {
    table = calloc(1, sizeof(*table));
    if (!table)
      return false;

    table->section_index = section_index;
    *out = table;
    return true;
  }

  if (note_sh->size > SIZE_MAX)
  {
    fprintf(stderr, "parse_elf_note_table: section %zu size too large\n", section_index);
    return false;
  }

  data = malloc((size_t)note_sh->size);
  if (!data)
  {
    fprintf(stderr, "parse_elf_note_table: out of memory\n");
    return false;
  }

  if (!file_seek(f, note_sh->offset))
  {
    free(data);
    return false;
  }

  if (file_read(f, data, (size_t)note_sh->size, (size_t)note_sh->size) != (int)note_sh->size)
  {
    fprintf(stderr, "parse_elf_note_table: failed to read section %zu\n", section_index);
    free(data);
    return false;
  }

  table = calloc(1, sizeof(*table));
  if (!table)
  {
    free(data);
    return false;
  }

  table->section_index = section_index;

  if (!parse_elf_notes_from_buffer(eh, data, (size_t)note_sh->size, table))
  {
    free(data);
    elf_note_table_destroy(table);
    return false;
  }

  free(data);
  *out = table;
  return true;
}

bool
parse_elf_note_tables(file_t *f, elf_header *eh, const elf_section_header_table *sections,
                      elf_note_table_set **out)
{
  size_t note_section_count = 0;
  elf_note_table_set *set;

  if (!f || !eh || !sections || !out)
  {
    fprintf(stderr, "parse_elf_note_tables: invalid argument\n");
    return false;
  }

  *out = NULL;

  for (size_t i = 0; i < sections->count; i++)
  {
    if (elf_section_header_is_type(sections, i, ELF_SHT_NOTE))
      note_section_count++;
  }

  set = calloc(1, sizeof(*set));
  if (!set)
    return false;

  if (note_section_count == 0)
  {
    *out = set;
    return true;
  }

  set->tables = calloc(note_section_count, sizeof(*set->tables));
  if (!set->tables)
  {
    free(set);
    return false;
  }

  for (size_t i = 0; i < sections->count; i++)
  {
    const elf_section_header *note_sh;

    if (!elf_section_header_is_type(sections, i, ELF_SHT_NOTE))
      continue;

    if (!elf_section_header_table_at(sections, i, &note_sh))
    {
      elf_note_table_set_destroy(set);
      return false;
    }

    if (!parse_elf_note_table(f, eh, note_sh, i, &set->tables[set->count]))
    {
      elf_note_table_set_destroy(set);
      return false;
    }

    set->count++;
  }

  *out = set;
  return true;
}
