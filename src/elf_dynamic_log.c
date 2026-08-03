#include "elf_dynamic_log.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "elf_endian.h"
#include "elf_section_header.h"

static bool
elf_dynamic_tag_uses_string(int64_t tag)
{
  switch (tag)
  {
  case ELF_DT_NEEDED:
  case ELF_DT_SONAME:
  case ELF_DT_RPATH:
  case ELF_DT_RUNPATH:
    return true;
  default:
    return false;
  }
}

static const char *
elf_dynamic_tag_name(int64_t tag)
{
  switch (tag)
  {
  case ELF_DT_NULL:
    return "NULL";
  case ELF_DT_NEEDED:
    return "NEEDED";
  case ELF_DT_PLTRELSZ:
    return "PLTRELSZ";
  case ELF_DT_PLTGOT:
    return "PLTGOT";
  case ELF_DT_HASH:
    return "HASH";
  case ELF_DT_STRTAB:
    return "STRTAB";
  case ELF_DT_SYMTAB:
    return "SYMTAB";
  case ELF_DT_RELA:
    return "RELA";
  case ELF_DT_RELASZ:
    return "RELASZ";
  case ELF_DT_RELAENT:
    return "RELAENT";
  case ELF_DT_STRSZ:
    return "STRSZ";
  case ELF_DT_SYMENT:
    return "SYMENT";
  case ELF_DT_INIT:
    return "INIT";
  case ELF_DT_FINI:
    return "FINI";
  case ELF_DT_SONAME:
    return "SONAME";
  case ELF_DT_RPATH:
    return "RPATH";
  case ELF_DT_SYMBOLIC:
    return "SYMBOLIC";
  case ELF_DT_REL:
    return "REL";
  case ELF_DT_RELSZ:
    return "RELSZ";
  case ELF_DT_RELENT:
    return "RELENT";
  case ELF_DT_PLTREL:
    return "PLTREL";
  case ELF_DT_DEBUG:
    return "DEBUG";
  case ELF_DT_TEXTREL:
    return "TEXTREL";
  case ELF_DT_JMPREL:
    return "JMPREL";
  case ELF_DT_BIND_NOW:
    return "BIND_NOW";
  case ELF_DT_INIT_ARRAY:
    return "INIT_ARRAY";
  case ELF_DT_FINI_ARRAY:
    return "FINI_ARRAY";
  case ELF_DT_INIT_ARRAYSZ:
    return "INIT_ARRAYSZ";
  case ELF_DT_FINI_ARRAYSZ:
    return "FINI_ARRAYSZ";
  case ELF_DT_RUNPATH:
    return "RUNPATH";
  case ELF_DT_FLAGS:
    return "FLAGS";
  case ELF_DT_PREINIT_ARRAY:
    return "PREINIT_ARRAY";
  case ELF_DT_PREINIT_ARRAYSZ:
    return "PREINIT_ARRAYSZ";
  case ELF_DT_RELRSZ:
    return "RELRSZ";
  case ELF_DT_RELR:
    return "RELR";
  case ELF_DT_RELRENT:
    return "RELRENT";
  case ELF_DT_GNU_HASH:
    return "GNU_HASH";
  case ELF_DT_VERSYM:
    return "VERSYM";
  case ELF_DT_FLAGS_1:
    return "FLAGS_1";
  case ELF_DT_VERDEF:
    return "VERDEF";
  case ELF_DT_VERDEFNUM:
    return "VERDEFNUM";
  case ELF_DT_VERNEED:
    return "VERNEED";
  case ELF_DT_VERNEEDNUM:
    return "VERNEEDNUM";
  default:
    return NULL;
  }
}

static void
log_dynamic_flags(uint64_t value, char *buf, size_t buf_size)
{
  size_t pos = 0;

  buf[0] = '\0';

  if (value & 0x1)
    pos += (size_t)snprintf(buf + pos, buf_size - pos, "%sORIGIN", pos ? " " : "");

  if (value & 0x2)
    pos += (size_t)snprintf(buf + pos, buf_size - pos, "%sTEXTREL", pos ? " " : "");

  if (value & 0x4)
    pos += (size_t)snprintf(buf + pos, buf_size - pos, "%sBIND_NOW", pos ? " " : "");

  if (pos == 0)
    snprintf(buf, buf_size, "0x%" PRIx64, value);
}

static void
log_dynamic_flags_1(uint64_t value, char *buf, size_t buf_size)
{
  size_t pos = 0;

  buf[0] = '\0';

  if (value & 0x1)
    pos += (size_t)snprintf(buf + pos, buf_size - pos, "%sNOW", pos ? " " : "");

  if (value & 0x2)
    pos += (size_t)snprintf(buf + pos, buf_size - pos, "%sPIE", pos ? " " : "");

  if (pos == 0)
    snprintf(buf, buf_size, "0x%" PRIx64, value);
}

static void
log_dynamic_value(const elf_dynamic *dynamic, const elf_dynamic_entry *entry, char *buf,
                  size_t buf_size)
{
  const char *str;

  if (elf_dynamic_tag_uses_string(entry->tag))
  {
    str = dynamic->dynstr ? elf_string_table_get(dynamic->dynstr, (uint32_t)entry->value) : NULL;
    if (str)
    {
      if (entry->tag == ELF_DT_NEEDED)
        snprintf(buf, buf_size, "Shared library: [%s]", str);
      else
        snprintf(buf, buf_size, "%s", str);
      return;
    }
  }

  if (entry->tag == ELF_DT_FLAGS)
  {
    log_dynamic_flags(entry->value, buf, buf_size);
    return;
  }

  if (entry->tag == ELF_DT_FLAGS_1)
  {
    char flags[64];

    log_dynamic_flags_1(entry->value, flags, sizeof(flags));
    snprintf(buf, buf_size, "Flags: %s", flags);
    return;
  }

  if (entry->tag == ELF_DT_NULL)
  {
    snprintf(buf, buf_size, "0x0");
    return;
  }

  if (entry->tag == ELF_DT_STRSZ || entry->tag == ELF_DT_RELASZ || entry->tag == ELF_DT_RELSZ
      || entry->tag == ELF_DT_INIT_ARRAYSZ || entry->tag == ELF_DT_FINI_ARRAYSZ
      || entry->tag == ELF_DT_PREINIT_ARRAYSZ || entry->tag == ELF_DT_RELRSZ
      || entry->tag == ELF_DT_SYMENT || entry->tag == ELF_DT_RELAENT || entry->tag == ELF_DT_RELENT
      || entry->tag == ELF_DT_RELRENT || entry->tag == ELF_DT_PLTRELSZ)
  {
    snprintf(buf, buf_size, "%" PRIu64 " bytes", entry->value);
    return;
  }

  snprintf(buf, buf_size, "0x%" PRIx64, entry->value);
}

void
log_elf_dynamic(const elf_header *eh, const elf_section_header_table *sections,
                const elf_string_table *shstrtab, const elf_dynamic *dynamic)
{
  int addr_width;
  char value_buf[128];

  if (!eh || !dynamic)
    return;

  addr_width = (int)elf_word_size(eh) * 2;

  printf("\nDynamic section at offset 0x%0*" PRIx64 " contains %zu entries:\n", addr_width,
         dynamic->offset, dynamic->count);
  printf("  Tag        Type                         Name/Value\n");

  for (size_t i = 0; i < dynamic->count; i++)
  {
    const elf_dynamic_entry *entry = &dynamic->entries[i];
    const char *type_name = elf_dynamic_tag_name(entry->tag);
    char type_field[48];

    log_dynamic_value(dynamic, entry, value_buf, sizeof(value_buf));

    if (type_name)
      snprintf(type_field, sizeof(type_field), "%s", type_name);
    else
      snprintf(type_field, sizeof(type_field), "0x%llx", (unsigned long long)entry->tag);

    printf(" 0x%0*" PRIx64 " %-28s %s\n", addr_width, (uint64_t)entry->tag, type_field,
           value_buf);
  }

  (void)sections;
  (void)shstrtab;
}

void
log_elf_needed_libraries(const elf_dynamic *dynamic)
{
  if (!dynamic)
    return;

  for (size_t i = 0; i < dynamic->count; i++)
  {
    const elf_dynamic_entry *entry = &dynamic->entries[i];
    const char *name;

    if (entry->tag != ELF_DT_NEEDED)
      continue;

    name = dynamic->dynstr ? elf_string_table_get(dynamic->dynstr, (uint32_t)entry->value) : NULL;
    if (!name || name[0] == '\0')
      continue;

    printf("%s\n", name);
  }
}
