#include "elf_program_header_log.h"

#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "elf_endian.h"
#include "elf_program_header.h"

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
get_elf_pt_type_str(uint32_t type)
{
  switch (type)
  {
  case ELF_PT_NULL:
    return "NULL";
  case ELF_PT_LOAD:
    return "LOAD";
  case ELF_PT_DYNAMIC:
    return "DYNAMIC";
  case ELF_PT_INTERP:
    return "INTERP";
  case ELF_PT_NOTE:
    return "NOTE";
  case ELF_PT_SHLIB:
    return "SHLIB";
  case ELF_PT_PHDR:
    return "PHDR";
  case ELF_PT_TLS:
    return "TLS";
  case ELF_PT_GNU_EH_FRAME:
    return "GNU_EH_FRAME";
  case ELF_PT_GNU_STACK:
    return "GNU_STACK";
  case ELF_PT_GNU_RELRO:
    return "GNU_RELRO";
  default:
    return "unknown";
  }
}

static void
log_elf_program_header_flags(uint32_t flags)
{
  char perm[4] = {' ', ' ', ' ', '\0'};

  if (flags & ELF_PF_R)
    perm[0] = 'R';
  if (flags & ELF_PF_W)
    perm[1] = 'W';
  if (flags & ELF_PF_X)
    perm[2] = 'X';

  log_field("Flags:", "0x%08" PRIx32 " (%s)", flags, perm);
}

void
log_elf_program_header_entry(const elf_header *eh, size_t index, const elf_program_header *entry)
{
  int addr_width;

  if (!eh || !entry)
    return;

  addr_width = (int)elf_word_size(eh) * 2;

  printf("Program Header %zu\n", index);

  {
    const char *type_name = get_elf_pt_type_str(entry->type);

    if (strcmp(type_name, "unknown") == 0)
      log_field("Type:", "0x%08" PRIx32, entry->type);
    else
      log_field("Type:", "%s (0x%08" PRIx32 ")", type_name, entry->type);
  }

  log_elf_program_header_flags(entry->flags);
  log_field("Offset:", "%" PRIu64 " (0x%0*" PRIx64 ")", entry->offset, addr_width, entry->offset);
  log_field("Virtual address:", "0x%0*" PRIx64, addr_width, entry->vaddr);
  log_field("Physical address:", "0x%0*" PRIx64, addr_width, entry->paddr);
  log_field("File size:", "%" PRIu64 " (0x%" PRIx64 ")", entry->filesz, entry->filesz);
  log_field("Memory size:", "%" PRIu64 " (0x%" PRIx64 ")", entry->memsz, entry->memsz);
  log_field("Align:", "%" PRIu64 " (0x%" PRIx64 ")", entry->align, entry->align);
}

void
log_elf_program_headers(const elf_header *eh, const elf_program_header_table *table)
{
  if (!eh || !table)
    return;

  printf("\nProgram Headers\n");

  if (table->count == 0)
  {
    printf("  (none)\n");
    return;
  }

  for (size_t i = 0; i < table->count; i++)
  {
    if (i > 0)
      printf("\n");

    log_elf_program_header_entry(eh, i, &table->entries[i]);
  }
}
