#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "elf_file_header.h"

bool is_elf_class_valid(elf_header *m);

const char *get_elf_class_str(elf_header *m);

bool is_elf_version_valid(elf_header *m);

const char *get_elf_version_str(elf_header *m);

bool is_elf_abi_valid(elf_header *m);

const char *get_elf_abi_str(elf_header *m);

bool is_elf_abi_version_valid(elf_header *m);

void get_elf_abi_version_str(elf_header *m, char *buf, size_t buf_size);

bool is_elf_type_valid(elf_header *m);

const char *get_elf_type_str(elf_header *m);

bool is_elf_machine_valid(elf_header *m);

const char *get_elf_machine_str(elf_header *m);

bool is_elf_header_version_valid(elf_header *m);

const char *get_elf_header_version_str(elf_header *m);
