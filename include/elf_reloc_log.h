#pragma once

#include "elf_file_header.h"
#include "elf_reloc.h"
#include "elf_section_header.h"
#include "elf_string_table.h"
#include "elf_symbol.h"

void log_elf_relocs(const elf_header *eh, const elf_section_header_table *sections,
                    const elf_string_table *shstrtab, const elf_symbol_table_set *symbols,
                    const elf_reloc_table_set *relocs);
