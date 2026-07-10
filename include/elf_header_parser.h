#pragma once

#include <stdbool.h>

#include "elf.h"
#include "file.h"

bool parse_elf_header(file_t *f, elf_header *header);
