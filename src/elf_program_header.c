#include "elf_program_header.h"

#include "elf_endian.h"

#include <stdio.h>
#include <stdlib.h>

static bool
seek_to_program_header_entry(file_t *f, elf_header *eh, size_t index)
{
  uint64_t offset;

  offset = eh->program_header_offset + (uint64_t)index * eh->ph_table_entry_size;
  return file_seek(f, offset);
}

bool
elf_program_header_table_create(size_t count, elf_program_header_table **out)
{
  if (!out)
  {
    fprintf(stderr, "elf_program_header_table_create: output pointer is null\n");
    return false;
  }

  elf_program_header_table *table = calloc(1, sizeof(*table));
  if (!table)
  {
    fprintf(stderr, "elf_program_header_table_create: out of memory\n");
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
    fprintf(stderr, "elf_program_header_table_create: out of memory\n");
    free(table);
    return false;
  }

  *out = table;
  return true;
}

void
elf_program_header_table_destroy(elf_program_header_table *table)
{
  if (!table)
    return;

  free(table->entries);
  free(table);
}

bool
parse_elf_program_header_entry(file_t *f, elf_header *eh, size_t index,
                                 elf_program_header *out)
{
  if (!f)
  {
    fprintf(stderr, "parse_elf_program_header_entry: file is null\n");
    return false;
  }

  if (!eh)
  {
    fprintf(stderr, "parse_elf_program_header_entry: elf header is null\n");
    return false;
  }

  if (!out)
  {
    fprintf(stderr, "parse_elf_program_header_entry: output pointer is null\n");
    return false;
  }

  if (index >= eh->ph_table_entry_count)
  {
    fprintf(stderr, "parse_elf_program_header_entry: index %zu out of range (count %u)\n",
            index, eh->ph_table_entry_count);
    return false;
  }

  if (!seek_to_program_header_entry(f, eh, index))
    return false;

  /* p_type */
  if (!elf_read_u32(f, eh, &out->type))
    return false;

  /* ELF64: p_flags follows p_type. ELF32: p_flags follows p_memsz. */
  if (eh->class == ELF_CLASS_64)
  {
    if (!elf_read_u32(f, eh, &out->flags))
      return false;
  }

  /* p_offset */
  if (!elf_read_word(f, eh, &out->offset))
    return false;

  /* p_vaddr */
  if (!elf_read_word(f, eh, &out->vaddr))
    return false;

  /* p_paddr */
  if (!elf_read_word(f, eh, &out->paddr))
    return false;

  /* p_filesz */
  if (!elf_read_word(f, eh, &out->filesz))
    return false;

  /* p_memsz */
  if (!elf_read_word(f, eh, &out->memsz))
    return false;

  if (eh->class == ELF_CLASS_32)
  {
    /* p_flags */
    if (!elf_read_u32(f, eh, &out->flags))
      return false;
  }

  /* p_align */
  if (!elf_read_word(f, eh, &out->align))
    return false;

  return true;
}

bool
parse_elf_program_headers(file_t *f, elf_header *eh, elf_program_header_table *table)
{
  if (!f)
  {
    fprintf(stderr, "parse_elf_program_headers: file is null\n");
    return false;
  }

  if (!eh)
  {
    fprintf(stderr, "parse_elf_program_headers: elf header is null\n");
    return false;
  }

  if (!table)
  {
    fprintf(stderr, "parse_elf_program_headers: table is null\n");
    return false;
  }

  if (table->count != eh->ph_table_entry_count)
  {
    fprintf(stderr,
            "parse_elf_program_headers: table count %zu does not match elf header count %u\n",
            table->count, eh->ph_table_entry_count);
    return false;
  }

  if (table->count > 0 && !table->entries)
  {
    fprintf(stderr, "parse_elf_program_headers: table entries is null\n");
    return false;
  }

  for (size_t i = 0; i < table->count; i++)
  {
    if (!parse_elf_program_header_entry(f, eh, i, &table->entries[i]))
      return false;
  }

  return true;
}
