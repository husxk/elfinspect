#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "elf_file_header.h"
#include "elf_section_header.h"
#include "file.h"

#define ELF_R_X86_64_NONE        0
#define ELF_R_X86_64_64          1
#define ELF_R_X86_64_PC32        2
#define ELF_R_X86_64_GOT32       3
#define ELF_R_X86_64_PLT32       4
#define ELF_R_X86_64_COPY        5
#define ELF_R_X86_64_GLOB_DAT    6
#define ELF_R_X86_64_JUMP_SLOT   7
#define ELF_R_X86_64_RELATIVE    8
#define ELF_R_X86_64_GOTPCREL    9
#define ELF_R_X86_64_32          10
#define ELF_R_X86_64_32S         11
#define ELF_R_X86_64_16          12
#define ELF_R_X86_64_PC16        13
#define ELF_R_X86_64_8           14
#define ELF_R_X86_64_PC8         15
#define ELF_R_X86_64_DTPMOD64    16
#define ELF_R_X86_64_DTPOFF64    17
#define ELF_R_X86_64_TPOFF64     18
#define ELF_R_X86_64_TLSGD       19
#define ELF_R_X86_64_TLSLD       20
#define ELF_R_X86_64_DTPOFF32    21
#define ELF_R_X86_64_GOTTPOFF    22
#define ELF_R_X86_64_TPOFF32     23
#define ELF_R_X86_64_PC64        24
#define ELF_R_X86_64_GOTOFF64    25
#define ELF_R_X86_64_GOTPC32     26
#define ELF_R_X86_64_GOT64       27
#define ELF_R_X86_64_GOTPCREL64  28
#define ELF_R_X86_64_GOTPC64     29
#define ELF_R_X86_64_GOTPLT64    30
#define ELF_R_X86_64_PLTOFF64    31
#define ELF_R_X86_64_SIZE32      32
#define ELF_R_X86_64_SIZE64      33
#define ELF_R_X86_64_RELA        36
#define ELF_R_X86_64_RELATIVE64  38
#define ELF_R_X86_64_IRELATIVE   37

typedef struct
{
  uint64_t offset;
  uint32_t sym;
  uint32_t type;
  int64_t addend;
} elf_reloc_entry;

typedef struct
{
  size_t section_index;
  size_t symtab_section_index;
  bool is_rela;
  uint64_t section_offset;
  elf_reloc_entry *entries;
  size_t count;
} elf_reloc_table;

typedef struct
{
  elf_reloc_table **tables;
  size_t count;
} elf_reloc_table_set;

void elf_reloc_table_destroy(elf_reloc_table *table);
void elf_reloc_table_set_destroy(elf_reloc_table_set *set);

bool parse_elf_reloc_tables(file_t *f, elf_header *eh, const elf_section_header_table *sections,
                            elf_reloc_table_set **out);
