#include "elf_header_parser.h"

#include <stdio.h>

#include "log.h"

static bool
parse_elf_magic(file_t *f, elf_header *header)
{
  if (!file_read(f, &header->magic, sizeof(header->magic), sizeof(header->magic)))
    return false;

  bool ret = is_elf_magic_valid(&header->magic);
  printf("%s magic\n", ret ? "Valid" : "Invalid");

  hexdump_quiet((unsigned char *) &header->magic, sizeof(header->magic));

  return ret;
}

static bool
parse_elf_class(file_t *f, elf_header *header)
{
  if (!file_read(f, &header->class, sizeof(header->class), sizeof(header->class)))
    return false;

  bool ret = is_elf_class_valid(header);
  printf("%s class %s\n", ret ? "Valid" : "Invalid", get_elf_class_str(header));

  return ret;
}

static bool
parse_elf_endian(file_t *f, elf_header *header)
{
  if (!file_read(f, &header->endian, sizeof(header->endian), sizeof(header->endian)))
    return false;

  bool ret = is_elf_endian_valid(header);
  printf("%s %s endian\n", ret ? "Valid" : "Invalid", get_elf_endian_str(header));

  return ret;
}

static bool
parse_elf_version(file_t *f, elf_header *header)
{
  if (!file_read(f, &header->version, sizeof(header->version), sizeof(header->version)))
    return false;

  bool ret = is_elf_version_valid(header);
  printf("%s version %s\n", ret ? "Valid" : "Invalid", get_elf_version_str(header));

  return ret;
}

static bool
parse_elf_abi(file_t *f, elf_header *header)
{
  if (!file_read(f, &header->abi, sizeof(header->abi), sizeof(header->abi)))
    return false;

  bool ret = is_elf_abi_valid(header);
  printf("%s ABI %s\n", ret ? "Valid" : "Invalid", get_elf_abi_str(header));

  return ret;
}

static bool
parse_elf_abi_version(file_t *f, elf_header *header)
{
  if (!file_read(f, &header->abi_version, sizeof(header->abi_version), sizeof(header->abi_version)))
    return false;

  bool ret = is_elf_abi_version_valid(header);
  char abi_version_str[16];

  get_elf_abi_version_str(header, abi_version_str, sizeof(abi_version_str));
  printf("%s ABI version %s\n", ret ? "Valid" : "Invalid", abi_version_str);

  return ret;
}

static bool
parse_elf_reserved(file_t *f, elf_header *header)
{
  if (!file_read(f, header->reserved, sizeof(header->reserved), sizeof(header->reserved)))
    return false;

  logit_hexdump("ELF reserved field", header->reserved, sizeof(header->reserved));

  return true;
}

static bool
parse_elf_type(file_t *f, elf_header *header)
{
  if (!file_read(f, &header->type, sizeof(header->type), sizeof(header->type)))
    return false;

  bool ret = is_elf_type_valid(header);
  printf("%s type %s\n", ret ? "Valid" : "Invalid", get_elf_type_str(header));

  return ret;
}

bool
parse_elf_header(file_t *f, elf_header *header)
{
  if (!parse_elf_magic(f, header))
    return false;

  if (!parse_elf_class(f, header))
    return false;

  if (!parse_elf_endian(f, header))
    return false;

  if (!parse_elf_version(f, header))
    return false;

  if (!parse_elf_abi(f, header))
    return false;

  if (!parse_elf_abi_version(f, header))
    return false;

  if (!parse_elf_reserved(f, header))
    return false;

  if (!parse_elf_type(f, header))
    return false;

  return true;
}
