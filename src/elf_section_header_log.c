#include "elf_section_header_log.h"

#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "elf_endian.h"
#include "elf_section_header.h"
#include "elf_string_table.h"

#define LABEL_WIDTH 36

static void
log_field(const char *label, const char *fmt, ...)
{
  char value[128];
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(value, sizeof(value), fmt, ap);
  va_end(ap);

  printf("  %-*s %s\n", LABEL_WIDTH, label, value);
}

static const char *
get_elf_sht_type_str(uint32_t type)
{
  switch (type)
  {
  case ELF_SHT_NULL:
    return "NULL";
  case ELF_SHT_PROGBITS:
    return "PROGBITS";
  case ELF_SHT_SYMTAB:
    return "SYMTAB";
  case ELF_SHT_STRTAB:
    return "STRTAB";
  case ELF_SHT_RELA:
    return "RELA";
  case ELF_SHT_HASH:
    return "HASH";
  case ELF_SHT_DYNAMIC:
    return "DYNAMIC";
  case ELF_SHT_NOTE:
    return "NOTE";
  case ELF_SHT_NOBITS:
    return "NOBITS";
  case ELF_SHT_REL:
    return "REL";
  case ELF_SHT_SHLIB:
    return "SHLIB";
  case ELF_SHT_DYNSYM:
    return "DYNSYM";
  case ELF_SHT_INIT_ARRAY:
    return "INIT_ARRAY";
  case ELF_SHT_FINI_ARRAY:
    return "FINI_ARRAY";
  case ELF_SHT_PREINIT_ARRAY:
    return "PREINIT_ARRAY";
  case ELF_SHT_GROUP:
    return "GROUP";
  case ELF_SHT_SYMTAB_SHNDX:
    return "SYMTAB_SHNDX";
  case ELF_SHT_GNU_ATTRIBUTES:
    return "GNU_ATTRIBUTES";
  case ELF_SHT_GNU_HASH:
    return "GNU_HASH";
  case ELF_SHT_GNU_LIBLIST:
    return "GNU_LIBLIST";
  case ELF_SHT_GNU_VERDEF:
    return "GNU_VERDEF";
  case ELF_SHT_GNU_VERNEED:
    return "GNU_VERNEED";
  case ELF_SHT_GNU_VERSYM:
    return "GNU_VERSYM";
  default:
    return "unknown";
  }
}

static void
log_elf_section_header_flags(uint64_t flags)
{
  char marks[16];
  size_t pos = 0;

  if (flags & ELF_SHF_WRITE)
    marks[pos++] = 'W';
  if (flags & ELF_SHF_ALLOC)
    marks[pos++] = 'A';
  if (flags & ELF_SHF_EXECINSTR)
    marks[pos++] = 'X';
  if (flags & ELF_SHF_MERGE)
    marks[pos++] = 'M';
  if (flags & ELF_SHF_STRINGS)
    marks[pos++] = 'S';
  if (pos == 0)
    marks[pos++] = ' ';
  marks[pos] = '\0';

  log_field("Flags:", "0x%016" PRIx64 " (%s)", flags, marks);
}

void
log_elf_section_header_entry(const elf_header *eh, const elf_string_table *shstrtab, size_t index,
                             const elf_section_header *entry)
{
  int addr_width;
  const char *name;

  if (!eh || !entry)
    return;

  addr_width = (int)elf_word_size(eh) * 2;

  printf("Section Header %zu\n", index);

  name = shstrtab ? elf_string_table_get(shstrtab, entry->name) : NULL;
  if (name)
    log_field("Name:", "%s (%" PRIu32 ")", name, entry->name);
  else
    log_field("Name index:", "%" PRIu32, entry->name);

  {
    const char *type_name = get_elf_sht_type_str(entry->type);

    if (strcmp(type_name, "unknown") == 0)
      log_field("Type:", "0x%08" PRIx32, entry->type);
    else
      log_field("Type:", "%s (0x%08" PRIx32 ")", type_name, entry->type);
  }

  log_elf_section_header_flags(entry->flags);
  log_field("Address:", "0x%0*" PRIx64, addr_width, entry->addr);
  log_field("Offset:", "%" PRIu64 " (0x%0*" PRIx64 ")", entry->offset, addr_width, entry->offset);
  log_field("Size:", "%" PRIu64 " (0x%" PRIx64 ")", entry->size, entry->size);
  log_field("Link:", "%" PRIu32, entry->link);
  log_field("Info:", "%" PRIu32, entry->info);
  log_field("Address alignment:", "%" PRIu64 " (0x%" PRIx64 ")", entry->addralign, entry->addralign);
  log_field("Entry size:", "%" PRIu64 " (0x%" PRIx64 ")", entry->entsize, entry->entsize);
}

void
log_elf_section_headers(const elf_header *eh, const elf_section_header_table *table,
                        const elf_string_table *shstrtab)
{
  if (!eh || !table)
    return;

  printf("\nSection Headers\n");

  if (table->count == 0)
  {
    printf("  (none)\n");
    return;
  }

  for (size_t i = 0; i < table->count; i++)
  {
    if (i > 0)
      printf("\n");

    log_elf_section_header_entry(eh, shstrtab, i, &table->entries[i]);
  }
}
