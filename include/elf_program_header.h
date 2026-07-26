#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "elf_file_header.h"
#include "file.h"

#define ELF_PT_NULL    0x0
#define ELF_PT_LOAD    0x1
#define ELF_PT_DYNAMIC 0x2
#define ELF_PT_INTERP  0x3
#define ELF_PT_NOTE    0x4
#define ELF_PT_SHLIB   0x5
#define ELF_PT_PHDR    0x6
#define ELF_PT_TLS     0x7
#define ELF_PT_GNU_EH_FRAME 0x6474e550
#define ELF_PT_GNU_STACK    0x6474e551
#define ELF_PT_GNU_RELRO    0x6474e552

#define ELF_PF_X 0x1
#define ELF_PF_W 0x2
#define ELF_PF_R 0x4

typedef struct
{
  uint32_t type;
  uint32_t flags;
  uint64_t offset;
  uint64_t vaddr;
  uint64_t paddr;
  uint64_t filesz;
  uint64_t memsz;
  uint64_t align;
} elf_program_header;

typedef struct
{
  size_t count;
  elf_program_header *entries;
} elf_program_header_table;

bool elf_program_header_table_create(size_t count, elf_program_header_table **out);
void elf_program_header_table_destroy(elf_program_header_table *table);

bool parse_elf_program_header_entry(file_t *f, elf_header *eh, size_t index,
                                      elf_program_header *out);
bool parse_elf_program_headers(file_t *f, elf_header *eh, elf_program_header_table *table);
