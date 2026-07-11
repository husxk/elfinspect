#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ELF_HEADER_RESERVED 7

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
} elf_header;

bool
is_elf_magic_valid(elf_magic *m);

bool
is_elf_class_valid(elf_header *m);

const char *
get_elf_class_str(elf_header *m);

bool
is_elf_version_valid(elf_header *m);

const char *
get_elf_version_str(elf_header *m);

bool
is_elf_abi_valid(elf_header *m);

const char *
get_elf_abi_str(elf_header *m);

bool
is_elf_abi_version_valid(elf_header *m);

void
get_elf_abi_version_str(elf_header *m, char *buf, size_t buf_size);

bool
is_elf_type_valid(elf_header *m);

const char *
get_elf_type_str(elf_header *m);

bool
is_elf_machine_valid(elf_header *m);

const char *
get_elf_machine_str(elf_header *m);

bool
is_elf_header_version_valid(elf_header *m);

const char *
get_elf_header_version_str(elf_header *m);
