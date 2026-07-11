#include "elf_header_parser.h"

#include <inttypes.h>
#include <stdio.h>

#include "elf_endian.h"
#include "log.h"

static bool
parse_elf_magic(file_t *f, elf_header *header)
{
  if (!file_read(f, header->magic.bytes, sizeof(header->magic.bytes), sizeof(header->magic.bytes)))
    return false;

  bool ret = is_elf_magic_valid(&header->magic);
  printf("%s magic\n", ret ? "Valid" : "Invalid");

  hexdump_quiet(header->magic.bytes, sizeof(header->magic.bytes));

  return ret;
}

static bool
parse_elf_class(file_t *f, elf_header *header)
{
  if (!file_read(f, &header->class, sizeof(header->class), sizeof(header->class)))
    return false;

  bool ret = is_elf_class_valid(header);
  printf("%s class %s\n", ret ? "Valid" : "Invalid", get_elf_class_str(header));

  return ret;
}

static bool
parse_elf_endian(file_t *f, elf_header *header)
{
  if (!file_read(f, &header->endian, sizeof(header->endian), sizeof(header->endian)))
    return false;

  bool ret = is_elf_endian_valid(header);
  printf("%s %s endian\n", ret ? "Valid" : "Invalid", get_elf_endian_str(header));

  return ret;
}

static bool
parse_elf_version(file_t *f, elf_header *header)
{
  if (!file_read(f, &header->version, sizeof(header->version), sizeof(header->version)))
    return false;

  bool ret = is_elf_version_valid(header);
  printf("%s version %s\n", ret ? "Valid" : "Invalid", get_elf_version_str(header));

  return ret;
}

static bool
parse_elf_abi(file_t *f, elf_header *header)
{
  if (!file_read(f, &header->abi, sizeof(header->abi), sizeof(header->abi)))
    return false;

  bool ret = is_elf_abi_valid(header);
  printf("%s ABI %s\n", ret ? "Valid" : "Invalid", get_elf_abi_str(header));

  return ret;
}

static bool
parse_elf_abi_version(file_t *f, elf_header *header)
{
  if (!file_read(f, &header->abi_version, sizeof(header->abi_version), sizeof(header->abi_version)))
    return false;

  bool ret = is_elf_abi_version_valid(header);
  char abi_version_str[16];

  get_elf_abi_version_str(header, abi_version_str, sizeof(abi_version_str));
  printf("%s ABI version %s\n", ret ? "Valid" : "Invalid", abi_version_str);

  return ret;
}

static bool
parse_elf_reserved(file_t *f, elf_header *header)
{
  if (!file_read(f, header->reserved, sizeof(header->reserved), sizeof(header->reserved)))
    return false;

  logit_hexdump("ELF reserved field", header->reserved, sizeof(header->reserved));

  return true;
}

static bool
parse_elf_type(file_t *f, elf_header *header)
{
  if (!elf_read_u16(f, header, &header->type))
    return false;

  bool ret = is_elf_type_valid(header);
  printf("%s type %s\n", ret ? "Valid" : "Invalid", get_elf_type_str(header));

  return ret;
}

static bool
parse_elf_machine(file_t *f, elf_header *header)
{
  if (!elf_read_u16(f, header, &header->machine))
    return false;

  bool ret = is_elf_machine_valid(header);
  printf("%s machine %s\n", ret ? "Valid" : "Invalid", get_elf_machine_str(header));

  return ret;
}

static bool
parse_elf_header_version(file_t *f, elf_header *header)
{
  if (!elf_read_u32(f, header, &header->header_version))
    return false;

  bool ret = is_elf_header_version_valid(header);
  printf("%s header version %s\n", ret ? "Valid" : "Invalid", get_elf_header_version_str(header));

  return ret;
}

static bool
parse_elf_entry(file_t *f, elf_header *header)
{
  if (!elf_read_word(f, header, &header->entry))
    return false;

  printf("Entry point address 0x%0*" PRIx64 "\n", (int)elf_word_size(header) * 2, header->entry);

  return true;
}

static bool
parse_elf_program_header_offset(file_t *f, elf_header *header)
{
  if (!elf_read_word(f, header, &header->program_header_offset))
    return false;

  printf("Program header table offset %" PRIu64 " (0x%" PRIx64 ")\n",
         header->program_header_offset, header->program_header_offset);

  return true;
}

static bool
parse_elf_section_header_offset(file_t *f, elf_header *header)
{
  if (!elf_read_word(f, header, &header->section_header_offset))
    return false;

  printf("Section header table offset %" PRIu64 " (0x%" PRIx64 ")\n",
         header->section_header_offset, header->section_header_offset);

  return true;
}

static bool
parse_elf_flags(file_t *f, elf_header *header)
{
  if (!elf_read_u32(f, header, &header->flags))
    return false;

  printf("Flags 0x%" PRIx32 "\n", header->flags);

  return true;
}

static bool
parse_elf_header_size(file_t *f, elf_header *header)
{
  if (!elf_read_u16(f, header, &header->elf_header_size))
    return false;

  printf("ELF header size %u\n", header->elf_header_size);

  return true;
}

static bool
parse_elf_ph_table_entry_size(file_t *f, elf_header *header)
{
  if (!elf_read_u16(f, header, &header->ph_table_entry_size))
    return false;

  printf("Program header table entry size %u\n", header->ph_table_entry_size);

  return true;
}

static bool
parse_elf_ph_table_entry_count(file_t *f, elf_header *header)
{
  if (!elf_read_u16(f, header, &header->ph_table_entry_count))
    return false;

  printf("Program header table entry count %u\n", header->ph_table_entry_count);

  return true;
}

static bool
parse_elf_sh_table_entry_size(file_t *f, elf_header *header)
{
  if (!elf_read_u16(f, header, &header->sh_table_entry_size))
    return false;

  printf("Section header table entry size %u\n", header->sh_table_entry_size);

  return true;
}

static bool
parse_elf_sh_table_entry_count(file_t *f, elf_header *header)
{
  if (!elf_read_u16(f, header, &header->sh_table_entry_count))
    return false;

  printf("Section header table entry count %u\n", header->sh_table_entry_count);

  return true;
}

static bool
parse_elf_sh_string_table_index(file_t *f, elf_header *header)
{
  if (!elf_read_u16(f, header, &header->sh_string_table_index))
    return false;

  printf("Section header string table index %u\n", header->sh_string_table_index);

  return true;
}

bool
parse_elf_header(file_t *f, elf_header *header)
{
  if (!parse_elf_magic(f, header))
    return false;

  if (!parse_elf_class(f, header))
    return false;

  if (!parse_elf_endian(f, header))
    return false;

  if (!parse_elf_version(f, header))
    return false;

  if (!parse_elf_abi(f, header))
    return false;

  if (!parse_elf_abi_version(f, header))
    return false;

  if (!parse_elf_reserved(f, header))
    return false;

  if (!parse_elf_type(f, header))
    return false;

  if (!parse_elf_machine(f, header))
    return false;

  if (!parse_elf_header_version(f, header))
    return false;

  if (!parse_elf_entry(f, header))
    return false;

  if (!parse_elf_program_header_offset(f, header))
    return false;

  if (!parse_elf_section_header_offset(f, header))
    return false;

  if (!parse_elf_flags(f, header))
    return false;

  if (!parse_elf_header_size(f, header))
    return false;

  if (!parse_elf_ph_table_entry_size(f, header))
    return false;

  if (!parse_elf_ph_table_entry_count(f, header))
    return false;

  if (!parse_elf_sh_table_entry_size(f, header))
    return false;

  if (!parse_elf_sh_table_entry_count(f, header))
    return false;

  if (!parse_elf_sh_string_table_index(f, header))
    return false;

  return true;
}
