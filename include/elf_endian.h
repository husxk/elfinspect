#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "elf_file_header.h"
#include "file.h"

#define ELF_ENDIAN_LITTLE 0x01
#define ELF_ENDIAN_BIG    0x02

bool elf_read_u16(file_t *f, elf_header *header, uint16_t *out);
bool elf_read_u32(file_t *f, elf_header *header, uint32_t *out);
bool elf_read_u64(file_t *f, elf_header *header, uint64_t *out);

size_t elf_word_size(const elf_header *header);
bool elf_read_word(file_t *f, elf_header *header, uint64_t *out);
