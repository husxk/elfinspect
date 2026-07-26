#include "elf_file_header.h"

#include "elf_endian.h"

#include <stdio.h>
#include <string.h>

static const char elf_magic_valid[] = "\x7f" "ELF";

bool
is_elf_magic_valid(elf_magic *m)
{
  if (!m)
    return false;

  return memcmp(m->bytes, elf_magic_valid, sizeof(m->bytes)) == 0;
}

bool
parse_elf_header(file_t *f, elf_header *header)
{
  if (!f)
  {
    fprintf(stderr, "parse_elf_header: file is null\n");
    return false;
  }

  if (!header)
  {
    fprintf(stderr, "parse_elf_header: header is null\n");
    return false;
  }

  if (!file_seek(f, 0))
    return false;

  /* e_ident: magic */
  if (!file_read(f, header->magic.bytes, sizeof(header->magic.bytes), sizeof(header->magic.bytes)))
    return false;

  if (!is_elf_magic_valid(&header->magic))
    return false;

  /* e_ident: class */
  if (!file_read(f, &header->class, sizeof(header->class), sizeof(header->class)))
    return false;

  /* e_ident: endian */
  if (!file_read(f, &header->endian, sizeof(header->endian), sizeof(header->endian)))
    return false;

  /* e_ident: version */
  if (!file_read(f, &header->version, sizeof(header->version), sizeof(header->version)))
    return false;

  /* e_ident: OS/ABI */
  if (!file_read(f, &header->abi, sizeof(header->abi), sizeof(header->abi)))
    return false;

  /* e_ident: ABI version */
  if (!file_read(f, &header->abi_version, sizeof(header->abi_version), sizeof(header->abi_version)))
    return false;

  /* e_ident: padding */
  if (!file_read(f, header->reserved, sizeof(header->reserved), sizeof(header->reserved)))
    return false;

  /* e_type */
  if (!elf_read_u16(f, header, &header->type))
    return false;

  /* e_machine */
  if (!elf_read_u16(f, header, &header->machine))
    return false;

  /* e_version */
  if (!elf_read_u32(f, header, &header->header_version))
    return false;

  /* e_entry */
  if (!elf_read_word(f, header, &header->entry))
    return false;

  /* e_phoff */
  if (!elf_read_word(f, header, &header->program_header_offset))
    return false;

  /* e_shoff */
  if (!elf_read_word(f, header, &header->section_header_offset))
    return false;

  /* e_flags */
  if (!elf_read_u32(f, header, &header->flags))
    return false;

  /* e_ehsize */
  if (!elf_read_u16(f, header, &header->elf_header_size))
    return false;

  /* e_phentsize */
  if (!elf_read_u16(f, header, &header->ph_table_entry_size))
    return false;

  /* e_phnum */
  if (!elf_read_u16(f, header, &header->ph_table_entry_count))
    return false;

  /* e_shentsize */
  if (!elf_read_u16(f, header, &header->sh_table_entry_size))
    return false;

  /* e_shnum */
  if (!elf_read_u16(f, header, &header->sh_table_entry_count))
    return false;

  /* e_shstrndx */
  if (!elf_read_u16(f, header, &header->sh_string_table_index))
    return false;

  return true;
}
