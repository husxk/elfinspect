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

bool
parse_elf_header(file_t *f, elf_header *header)
{
  if (!parse_elf_magic(f, header))
    return false;

  if (!parse_elf_class(f, header))
    return false;

  if (!parse_elf_endian(f, header))
    return false;

  return true;
}
