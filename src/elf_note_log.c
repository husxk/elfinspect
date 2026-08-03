#include "elf_note_log.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "elf_endian.h"
#include "elf_section_header.h"

#include <endian.h>

static uint32_t
note_desc_u32(const elf_header *eh, const uint8_t *raw)
{
  uint32_t v;

  memcpy(&v, raw, sizeof(v));

  if (eh->endian == ELF_ENDIAN_LITTLE)
    return le32toh(v);

  return be32toh(v);
}

static bool
note_owner_is_gnu(const char *name)
{
  return name && strcmp(name, "GNU") == 0;
}

static const char *
note_type_description(const elf_note_entry *entry)
{
  if (!note_owner_is_gnu(entry->name))
    return NULL;

  switch (entry->type)
  {
  case ELF_NT_GNU_ABI_TAG:
    return "NT_GNU_ABI_TAG (ABI version tag)";
  case ELF_NT_GNU_BUILD_ID:
    return "NT_GNU_BUILD_ID (unique build ID bitstring)";
  case ELF_NT_GNU_PROPERTY_TYPE_0:
    return "NT_GNU_PROPERTY_TYPE_0";
  default:
    return NULL;
  }
}

static void
log_note_abi_tag(const elf_header *eh, const elf_note_entry *entry)
{
  uint32_t os;
  uint32_t abi0;
  uint32_t abi1;
  uint32_t abi2;
  const char *os_name;

  if (entry->descsz < 16 || !entry->desc)
    return;

  os = note_desc_u32(eh, entry->desc);
  abi0 = note_desc_u32(eh, entry->desc + 4);
  abi1 = note_desc_u32(eh, entry->desc + 8);
  abi2 = note_desc_u32(eh, entry->desc + 12);

  switch (os)
  {
  case 0:
    os_name = "Linux";
    break;
  case 1:
    os_name = "Hurd";
    break;
  case 2:
    os_name = "NetBSD";
    break;
  case 3:
    os_name = "GNU";
    break;
  case 4:
    os_name = "Solaris";
    break;
  case 5:
    os_name = "FreeBSD";
    break;
  default:
    os_name = "Unknown";
    break;
  }

  printf("    OS: %s, ABI: %u.%u.%u\n", os_name, abi0, abi1, abi2);
}

static void
log_note_build_id(const elf_note_entry *entry)
{
  printf("    Build ID: ");
  for (size_t i = 0; i < entry->descsz; i++)
    printf("%02x", entry->desc[i]);
  printf("\n");
}

static void
log_note_entry(const elf_header *eh, const elf_note_entry *entry)
{
  const char *desc = note_type_description(entry);
  const char *owner;

  owner = entry->name ? entry->name : "";

  if (desc)
    printf("  %-20s 0x%08" PRIx32 "\t%s\n", owner, (uint32_t)entry->descsz, desc);
  else
    printf("  %-20s 0x%08" PRIx32 "\t0x%08" PRIx32 "\n", owner, (uint32_t)entry->descsz,
           entry->type);

  if (note_owner_is_gnu(entry->name))
  {
    if (entry->type == ELF_NT_GNU_ABI_TAG)
      log_note_abi_tag(eh, entry);
    else if (entry->type == ELF_NT_GNU_BUILD_ID)
      log_note_build_id(entry);
  }
}

static void
log_elf_note_table(const elf_header *eh, const elf_section_header_table *sections,
                     const elf_string_table *shstrtab, const elf_note_table *table)
{
  const elf_section_header *note_sh;
  const char *section_name;

  if (!table || table->count == 0)
    return;

  section_name = NULL;
  if (shstrtab && sections && elf_section_header_table_at(sections, table->section_index, &note_sh))
    section_name = elf_string_table_get(shstrtab, note_sh->name);

  if (section_name && section_name[0] != '\0')
    printf("\nDisplaying notes found in: %s\n", section_name);
  else
    printf("\nDisplaying notes found in section %zu\n", table->section_index);

  printf("  Owner                Data size \tDescription\n");

  for (size_t i = 0; i < table->count; i++)
    log_note_entry(eh, &table->entries[i]);
}

void
log_elf_notes(const elf_header *eh, const elf_section_header_table *sections,
              const elf_string_table *shstrtab, const elf_note_table_set *notes)
{
  if (!eh || !notes)
    return;

  for (size_t i = 0; i < notes->count; i++)
    log_elf_note_table(eh, sections, shstrtab, notes->tables[i]);
}
