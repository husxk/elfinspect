#pragma once

#include "elf_dynamic.h"
#include "elf_file_header.h"
#include "elf_section_header.h"
#include "elf_string_table.h"

void log_elf_dynamic(const elf_header *eh, const elf_section_header_table *sections,
                     const elf_string_table *shstrtab, const elf_dynamic *dynamic);
