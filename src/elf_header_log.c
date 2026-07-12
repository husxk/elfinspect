#include "elf_header_log.h"

#include <ctype.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>

#include "elf_endian.h"

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

static void
log_magic(const elf_header *header)
{
  char ascii_str[sizeof(header->magic.bytes) + 1];
  char hex_str[sizeof(header->magic.bytes) * 3];
  char magic_str[sizeof(hex_str) + 4 + sizeof(ascii_str)];
  size_t pos = 0;

  for (size_t i = 0; i < sizeof(header->magic.bytes); i++)
  {
    ascii_str[i] = (char)(isprint(header->magic.bytes[i]) ? header->magic.bytes[i] : '.');

    if (i > 0)
      hex_str[pos++] = ' ';

    pos += (size_t)snprintf(hex_str + pos, sizeof(hex_str) - pos,
                            "%02x", header->magic.bytes[i]);
  }

  ascii_str[sizeof(header->magic.bytes)] = '\0';

  snprintf(magic_str, sizeof(magic_str), "%s   %s", hex_str, ascii_str);
  log_field("Magic:", "%s", magic_str);
}

void
log_elf_header(const elf_header *header)
{
  char abi_version_str[16];
  elf_header *h = (elf_header *)header;

  if (!header)
    return;

  printf("ELF Header\n");

  log_magic(header);
  log_field("Class:", "%s", get_elf_class_str(h));
  log_field("Endian:", "%s", get_elf_endian_str(header));
  log_field("Version:", "%s", get_elf_version_str(h));
  log_field("OS/ABI:", "%s", get_elf_abi_str(h));

  get_elf_abi_version_str(h, abi_version_str, sizeof(abi_version_str));
  log_field("ABI version:", "%s", abi_version_str);

  log_field("Type:", "%s", get_elf_type_str(h));
  log_field("Machine:", "%s", get_elf_machine_str(h));
  log_field("Header version:", "%s", get_elf_header_version_str(h));
  log_field("Entry point address:", "0x%0*" PRIx64,
            (int)elf_word_size(header) * 2, header->entry);
  log_field("Program header table offset:", "%" PRIu64 " (0x%" PRIx64 ")",
            header->program_header_offset, header->program_header_offset);
  log_field("Section header table offset:", "%" PRIu64 " (0x%" PRIx64 ")",
            header->section_header_offset, header->section_header_offset);
  log_field("Flags:", "0x%" PRIx32, header->flags);
  log_field("ELF header size:", "%u", header->elf_header_size);
  log_field("Program header table entry size:", "%u", header->ph_table_entry_size);
  log_field("Program header table entry count:", "%u", header->ph_table_entry_count);
  log_field("Section header table entry size:", "%u", header->sh_table_entry_size);
  log_field("Section header table entry count:", "%u", header->sh_table_entry_count);
  log_field("Section header string table index:", "%u", header->sh_string_table_index);
}
