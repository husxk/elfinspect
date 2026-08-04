#include "elf_reloc_log.h"

#include <inttypes.h>
#include <stdio.h>

#include "elf_endian.h"
#include "elf_reloc.h"
#include "elf_section_header.h"

#define ELF_MACHINE_X86_64 62

static const char *
x86_64_reloc_type_name(uint32_t type)
{
  switch (type)
  {
  case ELF_R_X86_64_NONE:
    return "R_X86_64_NONE";
  case ELF_R_X86_64_64:
    return "R_X86_64_64";
  case ELF_R_X86_64_PC32:
    return "R_X86_64_PC32";
  case ELF_R_X86_64_GOT32:
    return "R_X86_64_GOT32";
  case ELF_R_X86_64_PLT32:
    return "R_X86_64_PLT32";
  case ELF_R_X86_64_COPY:
    return "R_X86_64_COPY";
  case ELF_R_X86_64_GLOB_DAT:
    return "R_X86_64_GLOB_DAT";
  case ELF_R_X86_64_JUMP_SLOT:
    return "R_X86_64_JUMP_SLOT";
  case ELF_R_X86_64_RELATIVE:
    return "R_X86_64_RELATIVE";
  case ELF_R_X86_64_GOTPCREL:
    return "R_X86_64_GOTPCREL";
  case ELF_R_X86_64_32:
    return "R_X86_64_32";
  case ELF_R_X86_64_32S:
    return "R_X86_64_32S";
  case ELF_R_X86_64_16:
    return "R_X86_64_16";
  case ELF_R_X86_64_PC16:
    return "R_X86_64_PC16";
  case ELF_R_X86_64_8:
    return "R_X86_64_8";
  case ELF_R_X86_64_PC8:
    return "R_X86_64_PC8";
  case ELF_R_X86_64_DTPMOD64:
    return "R_X86_64_DTPMOD64";
  case ELF_R_X86_64_DTPOFF64:
    return "R_X86_64_DTPOFF64";
  case ELF_R_X86_64_TPOFF64:
    return "R_X86_64_TPOFF64";
  case ELF_R_X86_64_TLSGD:
    return "R_X86_64_TLSGD";
  case ELF_R_X86_64_TLSLD:
    return "R_X86_64_TLSLD";
  case ELF_R_X86_64_DTPOFF32:
    return "R_X86_64_DTPOFF32";
  case ELF_R_X86_64_GOTTPOFF:
    return "R_X86_64_GOTTPOFF";
  case ELF_R_X86_64_TPOFF32:
    return "R_X86_64_TPOFF32";
  case ELF_R_X86_64_PC64:
    return "R_X86_64_PC64";
  case ELF_R_X86_64_GOTOFF64:
    return "R_X86_64_GOTOFF64";
  case ELF_R_X86_64_GOTPC32:
    return "R_X86_64_GOTPC32";
  case ELF_R_X86_64_GOT64:
    return "R_X86_64_GOT64";
  case ELF_R_X86_64_GOTPCREL64:
    return "R_X86_64_GOTPCREL64";
  case ELF_R_X86_64_GOTPC64:
    return "R_X86_64_GOTPC64";
  case ELF_R_X86_64_GOTPLT64:
    return "R_X86_64_GOTPLT64";
  case ELF_R_X86_64_PLTOFF64:
    return "R_X86_64_PLTOFF64";
  case ELF_R_X86_64_SIZE32:
    return "R_X86_64_SIZE32";
  case ELF_R_X86_64_SIZE64:
    return "R_X86_64_SIZE64";
  case ELF_R_X86_64_RELA:
    return "R_X86_64_RELA";
  case ELF_R_X86_64_RELATIVE64:
    return "R_X86_64_RELATIVE64";
  case ELF_R_X86_64_IRELATIVE:
    return "R_X86_64_IRELATIVE";
  default:
    return NULL;
  }
}

static const char *
reloc_type_name(const elf_header *eh, uint32_t type)
{
  if (eh->machine == ELF_MACHINE_X86_64)
    return x86_64_reloc_type_name(type);

  return NULL;
}

static uint64_t
reloc_info_field(const elf_header *eh, const elf_reloc_entry *entry)
{
  if (eh->class == ELF_CLASS_64)
    return ((uint64_t)entry->sym << 32) | entry->type;

  return ((uint64_t)entry->sym << 8) | entry->type;
}

static const elf_symbol_table *
find_symbol_table(const elf_symbol_table_set *symbols, size_t section_index)
{
  if (!symbols)
    return NULL;

  for (size_t i = 0; i < symbols->count; i++)
  {
    if (symbols->tables[i]->section_index == section_index)
      return symbols->tables[i];
  }

  return NULL;
}

static void
log_reloc_entry(const elf_header *eh, const elf_symbol_table *symtab, const elf_reloc_table *table,
                const elf_reloc_entry *entry)
{
  const char *type_name;
  const char *sym_name;
  uint64_t sym_value;
  uint64_t info;
  int addr_width;

  addr_width = (int)elf_word_size(eh) * 2;
  type_name = reloc_type_name(eh, entry->type);
  info = reloc_info_field(eh, entry);

  sym_name = "";
  sym_value = 0;

  if (symtab && entry->sym < symtab->count)
  {
    const elf_symbol *sym = &symtab->entries[entry->sym];

    sym_value = sym->value;
    if (symtab->strtab)
    {
      const char *n = elf_string_table_get(symtab->strtab, sym->name);

      if (n)
        sym_name = n;
    }
  }

  if (type_name)
  {
    printf("%0*" PRIx64 " %012" PRIx64 " %-17s %0*" PRIx64 " %s", addr_width, entry->offset, info,
           type_name, addr_width, sym_value, sym_name);
  }
  else
  {
    printf("%0*" PRIx64 " %012" PRIx64 " %-17" PRIu32 " %0*" PRIx64 " %s", addr_width,
           entry->offset, info, entry->type, addr_width, sym_value, sym_name);
  }

  if (table->is_rela)
    printf(" + %" PRId64, entry->addend);

  printf("\n");
}

static void
log_elf_reloc_table(const elf_header *eh, const elf_section_header_table *sections,
                    const elf_string_table *shstrtab, const elf_symbol_table_set *symbols,
                    const elf_reloc_table *table)
{
  const elf_section_header *reloc_sh;
  const char *section_name;
  const elf_symbol_table *symtab;
  int addr_width;

  if (!table || table->count == 0)
    return;

  section_name = NULL;
  if (shstrtab && sections
      && elf_section_header_table_at(sections, table->section_index, &reloc_sh))
    section_name = elf_string_table_get(shstrtab, reloc_sh->name);

  symtab = find_symbol_table(symbols, table->symtab_section_index);
  addr_width = (int)elf_word_size(eh) * 2;

  if (section_name && section_name[0] != '\0')
  {
    printf("\nRelocation section '%s' at offset 0x%0*" PRIx64 " contains %zu entries:\n",
           section_name, addr_width, table->section_offset, table->count);
  }
  else
  {
    printf("\nRelocation section %zu at offset 0x%0*" PRIx64 " contains %zu entries:\n",
           table->section_index, addr_width, table->section_offset, table->count);
  }

  if (table->is_rela)
    printf("  Offset          Info           Type           Sym. Value    Sym. Name + Addend\n");
  else
    printf("  Offset          Info           Type           Sym. Value    Sym. Name\n");

  for (size_t i = 0; i < table->count; i++)
    log_reloc_entry(eh, symtab, table, &table->entries[i]);
}

void
log_elf_relocs(const elf_header *eh, const elf_section_header_table *sections,
               const elf_string_table *shstrtab, const elf_symbol_table_set *symbols,
               const elf_reloc_table_set *relocs)
{
  if (!eh || !relocs)
    return;

  for (size_t i = 0; i < relocs->count; i++)
    log_elf_reloc_table(eh, sections, shstrtab, symbols, relocs->tables[i]);
}
