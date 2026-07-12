#include "elf_header_parser.h"

#include "elf_endian.h"
#include "file.h"

static bool
parse_elf_magic(file_t *f, elf_header *header)
{
  if (!file_read(f, header->magic.bytes, sizeof(header->magic.bytes), sizeof(header->magic.bytes)))
    return false;

  return is_elf_magic_valid(&header->magic);
}

static bool
parse_elf_class(file_t *f, elf_header *header)
{
  if (!file_read(f, &header->class, sizeof(header->class), sizeof(header->class)))
    return false;

  return true;
}

static bool
parse_elf_endian(file_t *f, elf_header *header)
{
  if (!file_read(f, &header->endian, sizeof(header->endian), sizeof(header->endian)))
    return false;

  return true;
}

static bool
parse_elf_version(file_t *f, elf_header *header)
{
  if (!file_read(f, &header->version, sizeof(header->version), sizeof(header->version)))
    return false;

  return true;
}

static bool
parse_elf_abi(file_t *f, elf_header *header)
{
  if (!file_read(f, &header->abi, sizeof(header->abi), sizeof(header->abi)))
    return false;

  return true;
}

static bool
parse_elf_abi_version(file_t *f, elf_header *header)
{
  if (!file_read(f, &header->abi_version, sizeof(header->abi_version), sizeof(header->abi_version)))
    return false;

  return true;
}

static bool
parse_elf_reserved(file_t *f, elf_header *header)
{
  if (!file_read(f, header->reserved, sizeof(header->reserved), sizeof(header->reserved)))
    return false;

  return true;
}

static bool
parse_elf_type(file_t *f, elf_header *header)
{
  if (!elf_read_u16(f, header, &header->type))
    return false;

  return true;
}

static bool
parse_elf_machine(file_t *f, elf_header *header)
{
  if (!elf_read_u16(f, header, &header->machine))
    return false;

  return true;
}

static bool
parse_elf_header_version(file_t *f, elf_header *header)
{
  if (!elf_read_u32(f, header, &header->header_version))
    return false;

  return true;
}

static bool
parse_elf_entry(file_t *f, elf_header *header)
{
  if (!elf_read_word(f, header, &header->entry))
    return false;

  return true;
}

static bool
parse_elf_program_header_offset(file_t *f, elf_header *header)
{
  if (!elf_read_word(f, header, &header->program_header_offset))
    return false;

  return true;
}

static bool
parse_elf_section_header_offset(file_t *f, elf_header *header)
{
  if (!elf_read_word(f, header, &header->section_header_offset))
    return false;

  return true;
}

static bool
parse_elf_flags(file_t *f, elf_header *header)
{
  if (!elf_read_u32(f, header, &header->flags))
    return false;

  return true;
}

static bool
parse_elf_header_size(file_t *f, elf_header *header)
{
  if (!elf_read_u16(f, header, &header->elf_header_size))
    return false;

  return true;
}

static bool
parse_elf_ph_table_entry_size(file_t *f, elf_header *header)
{
  if (!elf_read_u16(f, header, &header->ph_table_entry_size))
    return false;

  return true;
}

static bool
parse_elf_ph_table_entry_count(file_t *f, elf_header *header)
{
  if (!elf_read_u16(f, header, &header->ph_table_entry_count))
    return false;

  return true;
}

static bool
parse_elf_sh_table_entry_size(file_t *f, elf_header *header)
{
  if (!elf_read_u16(f, header, &header->sh_table_entry_size))
    return false;

  return true;
}

static bool
parse_elf_sh_table_entry_count(file_t *f, elf_header *header)
{
  if (!elf_read_u16(f, header, &header->sh_table_entry_count))
    return false;

  return true;
}

static bool
parse_elf_sh_string_table_index(file_t *f, elf_header *header)
{
  if (!elf_read_u16(f, header, &header->sh_string_table_index))
    return false;

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
