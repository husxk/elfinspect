#pragma once
#include <stdint.h>

typedef struct
{
  uint32_t magic;
} elf_magic;

typedef struct
{
  elf_magic magic;
  uint8_t class;
  uint8_t endian;
  uint8_t version;
  uint8_t abi;
} elf_header;

bool
is_elf_magic_valid(elf_magic *m);

bool
is_elf_class_valid(elf_header *m);

const char *
get_elf_class_str(elf_header *m);

bool
is_elf_endian_valid(elf_header *m);

const char *
get_elf_endian_str(elf_header *m);
