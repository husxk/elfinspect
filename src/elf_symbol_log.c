#include "elf_symbol_log.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "elf_endian.h"
#include "elf_section_header.h"

static const char *
elf_stb_to_string(uint8_t bind)
{
  switch (bind)
  {
  case ELF_STB_LOCAL:
    return "LOCAL";
  case ELF_STB_GLOBAL:
    return "GLOBAL";
  case ELF_STB_WEAK:
    return "WEAK";
  default:
    return "UNKNOWN";
  }
}

static const char *
elf_stt_to_string(uint8_t type)
{
  switch (type)
  {
  case ELF_STT_NOTYPE:
    return "NOTYPE";
  case ELF_STT_OBJECT:
    return "OBJECT";
  case ELF_STT_FUNC:
    return "FUNC";
  case ELF_STT_SECTION:
    return "SECTION";
  case ELF_STT_FILE:
    return "FILE";
  case ELF_STT_COMMON:
    return "COMMON";
  case ELF_STT_TLS:
    return "TLS";
  default:
    return "UNKNOWN";
  }
}

static const char *
elf_stv_to_string(uint8_t visibility)
{
  switch (visibility)
  {
  case ELF_STV_DEFAULT:
    return "DEFAULT";
  case ELF_STV_INTERNAL:
    return "INTERNAL";
  case ELF_STV_HIDDEN:
    return "HIDDEN";
  case ELF_STV_PROTECTED:
    return "PROTECTED";
  default:
    return "UNKNOWN";
  }
}

static void
format_symbol_shndx(const elf_header *eh, const elf_section_header_table *sections,
                    const elf_string_table *shstrtab, uint16_t shndx, char *buf, size_t buf_size)
{
  const elf_section_header *sh;
  const char *name;

  (void)eh;

  if (shndx == ELF_SHN_UNDEF)
  {
    snprintf(buf, buf_size, "UND");
    return;
  }

  if (shndx == ELF_SHN_ABS)
  {
    snprintf(buf, buf_size, "ABS");
    return;
  }

  if (shndx == ELF_SHN_COMMON)
  {
    snprintf(buf, buf_size, "COM");
    return;
  }

  if (shndx == ELF_SHN_XINDEX)
  {
    snprintf(buf, buf_size, "XINDEX");
    return;
  }

  if (shndx >= ELF_SHN_LORESERVE)
  {
    snprintf(buf, buf_size, "%u", shndx);
    return;
  }

  if (!elf_section_header_table_at(sections, shndx, &sh))
  {
    snprintf(buf, buf_size, "%u", shndx);
    return;
  }

  if (shstrtab)
  {
    name = elf_string_table_get(shstrtab, sh->name);
    if (name && name[0] != '\0')
    {
      snprintf(buf, buf_size, "%s", name);
      return;
    }
  }

  snprintf(buf, buf_size, "%u", shndx);
}

static void
log_elf_symbol_table(const elf_header *eh, const elf_section_header_table *sections,
                      const elf_string_table *shstrtab, const elf_symbol_table *table)
{
  const elf_section_header *symtab_sh;
  const char *section_name;
  int addr_width;

  if (!eh || !table)
    return;

  addr_width = (int)elf_word_size(eh) * 2;

  section_name = NULL;
  if (shstrtab && sections
      && elf_section_header_table_at(sections, table->section_index, &symtab_sh))
    section_name = elf_string_table_get(shstrtab, symtab_sh->name);

  if (section_name && section_name[0] != '\0')
    printf("\nSymbol table '%s' contains %zu entries:\n", section_name, table->count);
  else
    printf("\nSymbol table (section %zu) contains %zu entries:\n", table->section_index,
           table->count);

  printf("   Num:    Value          Size Type    Bind   Vis      Ndx Name\n");

  for (size_t i = 0; i < table->count; i++)
  {
    const elf_symbol *sym = &table->entries[i];
    const char *sym_name;
    char ndx_buf[64];

    sym_name = elf_string_table_get(table->strtab, sym->name);
    if (!sym_name)
      sym_name = "";

    format_symbol_shndx(eh, sections, shstrtab, sym->shndx, ndx_buf, sizeof(ndx_buf));

    printf("  %4zu: %0*" PRIx64 " %8" PRIu64 " %-7s %-6s %-8s %3s %s\n", i, addr_width,
           sym->value, sym->size, elf_stt_to_string(sym->type), elf_stb_to_string(sym->bind),
           elf_stv_to_string(sym->visibility), ndx_buf, sym_name);
  }
}

void
log_elf_symbols(const elf_header *eh, const elf_section_header_table *sections,
                const elf_string_table *shstrtab, const elf_symbol_table_set *symbols)
{
  if (!eh || !symbols)
    return;

  if (symbols->count == 0)
    return;

  for (size_t i = 0; i < symbols->count; i++)
    log_elf_symbol_table(eh, sections, shstrtab, symbols->tables[i]);
}
