#include "elf_dynamic.h"

#include "elf_endian.h"

#include <stdio.h>
#include <stdlib.h>

static size_t
elf_dynamic_entry_size_for_class(const elf_header *eh)
{
  if (eh->class == ELF_CLASS_64)
    return 16;

  if (eh->class == ELF_CLASS_32)
    return 8;

  return 0;
}

static bool
elf_dynamic_entry_size(const elf_header *eh, const elf_section_header *dyn_sh, size_t *out)
{
  size_t canonical;
  size_t entry_size;

  if (!out || !eh || !dyn_sh)
    return false;

  canonical = elf_dynamic_entry_size_for_class(eh);
  if (canonical == 0)
  {
    fprintf(stderr, "elf_dynamic_entry_size: invalid ELF class 0x%02x\n", eh->class);
    return false;
  }

  if (dyn_sh->entsize != 0)
    entry_size = (size_t)dyn_sh->entsize;
  else
    entry_size = canonical;

  if (entry_size != canonical)
  {
    fprintf(stderr,
            "elf_dynamic_entry_size: section entsize %zu does not match ELF class (expected %zu)\n",
            entry_size, canonical);
    return false;
  }

  *out = entry_size;
  return true;
}

static bool
parse_elf_dynamic_entry(file_t *f, elf_header *eh, elf_dynamic_entry *out)
{
  if (eh->class == ELF_CLASS_64)
  {
    uint64_t tag_raw;

    if (!elf_read_u64(f, eh, &tag_raw))
      return false;

    out->tag = (int64_t)tag_raw;

    if (!elf_read_u64(f, eh, &out->value))
      return false;
  }
  else if (eh->class == ELF_CLASS_32)
  {
    uint32_t tag32;
    uint32_t val32;

    if (!elf_read_u32(f, eh, &tag32))
      return false;

    out->tag = (int32_t)tag32;

    if (!elf_read_u32(f, eh, &val32))
      return false;

    out->value = val32;
  }
  else
  {
    fprintf(stderr, "parse_elf_dynamic_entry: invalid class 0x%02x\n", eh->class);
    return false;
  }

  return true;
}

static bool
find_dynamic_section_index(const elf_section_header_table *sections, size_t *out_index)
{
  if (!sections || !out_index)
    return false;

  for (size_t i = 0; i < sections->count; i++)
  {
    if (elf_section_header_is_type(sections, i, ELF_SHT_DYNAMIC))
    {
      *out_index = i;
      return true;
    }
  }

  return false;
}

void
elf_dynamic_destroy(elf_dynamic *dynamic)
{
  if (!dynamic)
    return;

  free(dynamic->entries);

  if (dynamic->dynstr)
    elf_string_table_destroy(dynamic->dynstr);

  free(dynamic);
}

bool
parse_elf_dynamic(file_t *f, elf_header *eh, const elf_section_header_table *sections,
                  elf_dynamic **out)
{
  const elf_section_header *dyn_sh;
  const elf_section_header *str_sh;
  size_t section_index;
  size_t entry_size;
  size_t count;
  elf_dynamic *dynamic;

  if (!f)
  {
    fprintf(stderr, "parse_elf_dynamic: file is null\n");
    return false;
  }

  if (!eh)
  {
    fprintf(stderr, "parse_elf_dynamic: elf header is null\n");
    return false;
  }

  if (!sections)
  {
    fprintf(stderr, "parse_elf_dynamic: section headers is null\n");
    return false;
  }

  if (!out)
  {
    fprintf(stderr, "parse_elf_dynamic: output pointer is null\n");
    return false;
  }

  *out = NULL;

  if (!find_dynamic_section_index(sections, &section_index))
    return true;

  if (!elf_section_header_table_at(sections, section_index, &dyn_sh))
    return false;

  if (!elf_dynamic_entry_size(eh, dyn_sh, &entry_size))
    return false;

  if (dyn_sh->size % entry_size != 0)
  {
    fprintf(stderr, "parse_elf_dynamic: section size %llu not multiple of entry size %zu\n",
            (unsigned long long)dyn_sh->size, entry_size);
    return false;
  }

  if (!elf_section_header_table_at(sections, dyn_sh->link, &str_sh))
    return false;

  if (str_sh->type != ELF_SHT_STRTAB)
  {
    fprintf(stderr, "parse_elf_dynamic: sh_link %u is not STRTAB\n", dyn_sh->link);
    return false;
  }

  dynamic = calloc(1, sizeof(*dynamic));
  if (!dynamic)
  {
    fprintf(stderr, "parse_elf_dynamic: out of memory\n");
    return false;
  }

  dynamic->section_index = section_index;
  dynamic->offset = dyn_sh->offset;

  if (!parse_elf_string_table(f, str_sh->offset, str_sh->size, &dynamic->dynstr))
  {
    elf_dynamic_destroy(dynamic);
    return false;
  }

  count = (size_t)(dyn_sh->size / entry_size);
  dynamic->count = count;

  if (count == 0)
  {
    *out = dynamic;
    return true;
  }

  dynamic->entries = calloc(count, sizeof(*dynamic->entries));
  if (!dynamic->entries)
  {
    fprintf(stderr, "parse_elf_dynamic: out of memory\n");
    elf_dynamic_destroy(dynamic);
    return false;
  }

  if (!file_seek(f, dyn_sh->offset))
  {
    elf_dynamic_destroy(dynamic);
    return false;
  }

  for (size_t i = 0; i < count; i++)
  {
    if (!parse_elf_dynamic_entry(f, eh, &dynamic->entries[i]))
    {
      fprintf(stderr, "parse_elf_dynamic: failed to parse entry %zu\n", i);
      elf_dynamic_destroy(dynamic);
      return false;
    }
  }

  *out = dynamic;
  return true;
}
