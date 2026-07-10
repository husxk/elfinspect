#include "elf.h"

#include <string.h>

static const char elf_magic_valid[] = "\x7f" "ELF";

#define ELF_CLASS_32 0x01
#define ELF_CLASS_64 0x02

#define ELF_ENDIAN_LITTLE 0x01
#define ELF_ENDIAN_BIG    0x02

bool
is_elf_magic_valid(elf_magic *m)
{
  if (!m)
    return false;

  return memcmp(&m->magic, elf_magic_valid, sizeof(m->magic)) == 0;
}

bool
is_elf_class_valid(elf_header *m)
{
  if (!m)
    return false;

  if (m->class == ELF_CLASS_32 ||
      m->class == ELF_CLASS_64)
    return true;

  return false;
}

const char *
get_elf_class_str(elf_header *m)
{
  if (!m)
    return "unknown";

  if (m->class == ELF_CLASS_32)
    return "ELF32";
  if (m->class == ELF_CLASS_64)
    return "ELF64";

  return "unrecognised";
}

bool
is_elf_endian_valid(elf_header *m)
{
  if (!m)
    return false;

  if (m->endian == ELF_ENDIAN_LITTLE ||
      m->endian == ELF_ENDIAN_BIG)
    return true;

  return false;
}

const char *
get_elf_endian_str(elf_header *m)
{
  if (!m)
    return "unknown";

  if (m->endian == ELF_ENDIAN_LITTLE)
    return "little";
  if (m->endian == ELF_ENDIAN_BIG)
    return "big";

  return "unrecognised";
}

