#include "elf_section_header.h"

#include "elf_endian.h"

#include <stdio.h>
#include <stdlib.h>

static bool
seek_to_section_header_entry(file_t *f, elf_header *eh, size_t index)
{
  uint64_t offset;

  offset = eh->section_header_offset + (uint64_t)index * eh->sh_table_entry_size;
  return file_seek(f, offset);
}

bool
elf_section_header_table_create(size_t count, elf_section_header_table **out)
{
  if (!out)
  {
    fprintf(stderr, "elf_section_header_table_create: output pointer is null\n");
    return false;
  }

  elf_section_header_table *table = calloc(1, sizeof(*table));
  if (!table)
  {
    fprintf(stderr, "elf_section_header_table_create: out of memory\n");
    return false;
  }

  table->count = count;

  if (count == 0)
  {
    table->entries = NULL;
    *out = table;
    return true;
  }

  table->entries = calloc(count, sizeof(*table->entries));
  if (!table->entries)
  {
    fprintf(stderr, "elf_section_header_table_create: out of memory\n");
    free(table);
    return false;
  }

  *out = table;
  return true;
}

void
elf_section_header_table_destroy(elf_section_header_table *table)
{
  if (!table)
    return;

  free(table->entries);
  free(table);
}

bool
parse_elf_section_header_entry(file_t *f, elf_header *eh, size_t index,
                                 elf_section_header *out)
{
  if (!f)
  {
    fprintf(stderr, "parse_elf_section_header_entry: file is null\n");
    return false;
  }

  if (!eh)
  {
    fprintf(stderr, "parse_elf_section_header_entry: elf header is null\n");
    return false;
  }

  if (!out)
  {
    fprintf(stderr, "parse_elf_section_header_entry: output pointer is null\n");
    return false;
  }

  if (index >= eh->sh_table_entry_count)
  {
    fprintf(stderr, "parse_elf_section_header_entry: index %zu out of range (count %u)\n",
            index, eh->sh_table_entry_count);
    return false;
  }

  if (!seek_to_section_header_entry(f, eh, index))
    return false;

  /* sh_name */
  if (!elf_read_u32(f, eh, &out->name))
    return false;

  /* sh_type */
  if (!elf_read_u32(f, eh, &out->type))
    return false;

  /* sh_flags */
  if (eh->class == ELF_CLASS_64)
  {
    if (!elf_read_u64(f, eh, &out->flags))
      return false;
  }
  else if (eh->class == ELF_CLASS_32)
  {
    uint32_t flags32;

    if (!elf_read_u32(f, eh, &flags32))
      return false;

    out->flags = flags32;
  }
  else
  {
    fprintf(stderr, "parse_elf_section_header_entry: invalid class 0x%02x\n", eh->class);
    return false;
  }

  /* sh_addr */
  if (!elf_read_word(f, eh, &out->addr))
    return false;

  /* sh_offset */
  if (!elf_read_word(f, eh, &out->offset))
    return false;

  /* sh_size */
  if (!elf_read_word(f, eh, &out->size))
    return false;

  /* sh_link */
  if (!elf_read_u32(f, eh, &out->link))
    return false;

  /* sh_info */
  if (!elf_read_u32(f, eh, &out->info))
    return false;

  /* sh_addralign */
  if (!elf_read_word(f, eh, &out->addralign))
    return false;

  /* sh_entsize */
  if (!elf_read_word(f, eh, &out->entsize))
    return false;

  return true;
}

bool
parse_elf_section_headers(file_t *f, elf_header *eh, elf_section_header_table *table)
{
  if (!f)
  {
    fprintf(stderr, "parse_elf_section_headers: file is null\n");
    return false;
  }

  if (!eh)
  {
    fprintf(stderr, "parse_elf_section_headers: elf header is null\n");
    return false;
  }

  if (!table)
  {
    fprintf(stderr, "parse_elf_section_headers: table is null\n");
    return false;
  }

  if (table->count != eh->sh_table_entry_count)
  {
    fprintf(stderr,
            "parse_elf_section_headers: table count %zu does not match elf header count %u\n",
            table->count, eh->sh_table_entry_count);
    return false;
  }

  if (table->count > 0 && !table->entries)
  {
    fprintf(stderr, "parse_elf_section_headers: table entries is null\n");
    return false;
  }

  for (size_t i = 0; i < table->count; i++)
  {
    if (!parse_elf_section_header_entry(f, eh, i, &table->entries[i]))
      return false;
  }

  return true;
}

bool
elf_section_header_is_type(const elf_section_header_table *table, size_t index, uint32_t type)
{
  if (!table || !table->entries)
    return false;

  if (index >= table->count)
    return false;

  return table->entries[index].type == type;
}

bool
elf_section_header_table_at(const elf_section_header_table *table, size_t index,
                          const elf_section_header **out)
{
  if (!out)
  {
    fprintf(stderr, "elf_section_header_table_at: output pointer is null\n");
    return false;
  }

  *out = NULL;

  if (!table || !table->entries)
  {
    fprintf(stderr, "elf_section_header_table_at: table is null\n");
    return false;
  }

  if (index >= table->count)
  {
    fprintf(stderr, "elf_section_header_table_at: index %zu out of range (count %zu)\n", index,
            table->count);
    return false;
  }

  *out = &table->entries[index];
  return true;
}
