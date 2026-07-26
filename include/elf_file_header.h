#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "file.h"

#define ELF_HEADER_RESERVED 7

#define ELF_CLASS_32 0x01
#define ELF_CLASS_64 0x02

typedef struct
{
  uint8_t bytes[4];
} elf_magic;

typedef struct
{
  elf_magic magic;
  uint8_t class;
  uint8_t endian;
  uint8_t version;
  uint8_t abi;
  uint8_t abi_version;
  uint8_t reserved[ELF_HEADER_RESERVED];
  uint16_t type;
  uint16_t machine;
  uint32_t header_version;
  uint64_t entry;
  uint64_t program_header_offset;
  uint64_t section_header_offset;
  uint32_t flags;
  uint16_t elf_header_size;
  uint16_t ph_table_entry_size;
  uint16_t ph_table_entry_count;
  uint16_t sh_table_entry_size;
  uint16_t sh_table_entry_count;
  uint16_t sh_string_table_index;
} elf_header;

bool is_elf_magic_valid(elf_magic *m);

bool parse_elf_header(file_t *f, elf_header *header);
