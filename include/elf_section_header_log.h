#pragma once

#include "elf.h"
#include "elf_section_header.h"

void log_elf_section_header_entry(const elf_header *eh, size_t index,
                                  const elf_section_header *entry);
void log_elf_section_headers(const elf_header *eh, const elf_section_header_table *table);
