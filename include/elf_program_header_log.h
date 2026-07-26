#pragma once

#include "elf_file_header.h"
#include "elf_program_header.h"

void log_elf_program_header_entry(const elf_header *eh, size_t index,
                                  const elf_program_header *entry);
void log_elf_program_headers(const elf_header *eh, const elf_program_header_table *table);
