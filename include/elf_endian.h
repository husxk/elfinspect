#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "elf.h"
#include "file.h"

#define ELF_ENDIAN_LITTLE 0x01
#define ELF_ENDIAN_BIG    0x02

bool is_elf_endian_valid(const elf_header *header);
const char *get_elf_endian_str(const elf_header *header);

bool elf_read_u16(file_t *f, elf_header *header, uint16_t *out);
bool elf_read_u32(file_t *f, elf_header *header, uint32_t *out);
bool elf_read_u64(file_t *f, elf_header *header, uint64_t *out);
