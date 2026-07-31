#pragma once

#include "elf_file_header.h"
#include "elf_section_header.h"
#include "elf_string_table.h"

void log_elf_section_header_entry(const elf_header *eh, const elf_string_table *shstrtab,
                                  size_t index, const elf_section_header *entry);
void log_elf_section_headers(const elf_header *eh, const elf_section_header_table *table,
                             const elf_string_table *shstrtab);
