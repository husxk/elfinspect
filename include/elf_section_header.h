#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "elf_file_header.h"
#include "file.h"

#define ELF_SHT_NULL         0x0
#define ELF_SHT_PROGBITS     0x1
#define ELF_SHT_SYMTAB       0x2
#define ELF_SHT_STRTAB       0x3
#define ELF_SHT_RELA         0x4
#define ELF_SHT_HASH         0x5
#define ELF_SHT_DYNAMIC      0x6
#define ELF_SHT_NOTE         0x7
#define ELF_SHT_NOBITS       0x8
#define ELF_SHT_REL          0x9
#define ELF_SHT_SHLIB        0x0a
#define ELF_SHT_DYNSYM       0x0b
#define ELF_SHT_INIT_ARRAY   0x0e
#define ELF_SHT_FINI_ARRAY   0x0f
#define ELF_SHT_PREINIT_ARRAY 0x10
#define ELF_SHT_GROUP        0x11
#define ELF_SHT_SYMTAB_SHNDX 0x12
#define ELF_SHT_GNU_ATTRIBUTES 0x6ffffff5
#define ELF_SHT_GNU_HASH     0x6ffffff6
#define ELF_SHT_GNU_LIBLIST  0x6ffffff7
#define ELF_SHT_GNU_VERDEF   0x6ffffffd
#define ELF_SHT_GNU_VERNEED  0x6ffffffe
#define ELF_SHT_GNU_VERSYM   0x6fffffff

#define ELF_SHF_WRITE            0x1
#define ELF_SHF_ALLOC            0x2
#define ELF_SHF_EXECINSTR        0x4
#define ELF_SHF_MERGE            0x10
#define ELF_SHF_STRINGS          0x20
#define ELF_SHF_INFO_LINK        0x40
#define ELF_SHF_LINK_ORDER       0x80
#define ELF_SHF_OS_NONCONFORMING 0x100
#define ELF_SHF_GROUP            0x200
#define ELF_SHF_TLS              0x400

#define ELF_SHN_UNDEF     0x0000
#define ELF_SHN_LORESERVE 0xff00
#define ELF_SHN_LOPROC    0xff00
#define ELF_SHN_HIPROC    0xff1f
#define ELF_SHN_LOOS      0xff20
#define ELF_SHN_HIOS      0xff3f
#define ELF_SHN_ABS       0xfff1
#define ELF_SHN_COMMON    0xfff2
#define ELF_SHN_XINDEX    0xffff

typedef struct
{
  uint32_t name;
  uint32_t type;
  uint64_t flags;
  uint64_t addr;
  uint64_t offset;
  uint64_t size;
  uint32_t link;
  uint32_t info;
  uint64_t addralign;
  uint64_t entsize;
} elf_section_header;

typedef struct
{
  size_t count;
  elf_section_header *entries;
} elf_section_header_table;

bool elf_section_header_table_create(size_t count, elf_section_header_table **out);
void elf_section_header_table_destroy(elf_section_header_table *table);

bool parse_elf_section_header_entry(file_t *f, elf_header *eh, size_t index,
                                      elf_section_header *out);
bool parse_elf_section_headers(file_t *f, elf_header *eh, elf_section_header_table *table);

bool elf_section_header_is_type(const elf_section_header_table *table, size_t index,
                                uint32_t type);

bool elf_section_header_table_at(const elf_section_header_table *table, size_t index,
                                 const elf_section_header **out);
