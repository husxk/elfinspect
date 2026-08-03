#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "elf_file_header.h"
#include "elf_section_header.h"
#include "elf_string_table.h"
#include "file.h"

#define ELF_DT_NULL            0
#define ELF_DT_NEEDED          1
#define ELF_DT_PLTRELSZ        2
#define ELF_DT_PLTGOT          3
#define ELF_DT_HASH            4
#define ELF_DT_STRTAB          5
#define ELF_DT_SYMTAB          6
#define ELF_DT_RELA            7
#define ELF_DT_RELASZ          8
#define ELF_DT_RELAENT         9
#define ELF_DT_STRSZ           10
#define ELF_DT_SYMENT          11
#define ELF_DT_INIT            12
#define ELF_DT_FINI            13
#define ELF_DT_SONAME          14
#define ELF_DT_RPATH           15
#define ELF_DT_SYMBOLIC        16
#define ELF_DT_REL             17
#define ELF_DT_RELSZ           18
#define ELF_DT_RELENT          19
#define ELF_DT_PLTREL          20
#define ELF_DT_DEBUG           21
#define ELF_DT_TEXTREL         22
#define ELF_DT_JMPREL          23
#define ELF_DT_BIND_NOW        24
#define ELF_DT_INIT_ARRAY      25
#define ELF_DT_FINI_ARRAY      26
#define ELF_DT_INIT_ARRAYSZ    27
#define ELF_DT_FINI_ARRAYSZ    28
#define ELF_DT_RUNPATH         29
#define ELF_DT_FLAGS           30
#define ELF_DT_ENCODING        32
#define ELF_DT_PREINIT_ARRAY   32
#define ELF_DT_PREINIT_ARRAYSZ 33
#define ELF_DT_RELRSZ          35
#define ELF_DT_RELR            36
#define ELF_DT_RELRENT         37

#define ELF_DT_GNU_HASH        0x6ffffef5
#define ELF_DT_TLSDESC_PLT     0x6ffffef6
#define ELF_DT_TLSDESC_GOT     0x6ffffef7
#define ELF_DT_GNU_CONFLICT    0x6ffffef8
#define ELF_DT_GNU_LIBLIST     0x6ffffef9
#define ELF_DT_CONFIG          0x6ffffefa
#define ELF_DT_DEPAUDIT        0x6ffffefb
#define ELF_DT_AUDIT           0x6ffffefc
#define ELF_DT_PLTPAD          0x6ffffefd
#define ELF_DT_MOVETAB         0x6ffffefe
#define ELF_DT_SYMINFO         0x6ffffeff
#define ELF_DT_VERSYM          0x6ffffff0
#define ELF_DT_RELACOUNT       0x6ffffff9
#define ELF_DT_RELCOUNT        0x6ffffffa
#define ELF_DT_FLAGS_1         0x6ffffffb
#define ELF_DT_VERDEF          0x6ffffffc
#define ELF_DT_VERDEFNUM       0x6ffffffd
#define ELF_DT_VERNEED         0x6ffffffe
#define ELF_DT_VERNEEDNUM      0x6fffffff

typedef struct
{
  int64_t tag;
  uint64_t value;
} elf_dynamic_entry;

typedef struct
{
  size_t section_index;
  uint64_t offset;
  elf_dynamic_entry *entries;
  size_t count;
  elf_string_table *dynstr;
} elf_dynamic;

void elf_dynamic_destroy(elf_dynamic *dynamic);

bool parse_elf_dynamic(file_t *f, elf_header *eh, const elf_section_header_table *sections,
                       elf_dynamic **out);
