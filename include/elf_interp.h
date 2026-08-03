#pragma once

#include <stdbool.h>

#include "elf_file_header.h"
#include "elf_program_header.h"
#include "file.h"

bool parse_elf_interp(file_t *f, const elf_header *eh, const elf_program_header_table *phdrs,
                      char **out);

void elf_interp_destroy(char *interp);
