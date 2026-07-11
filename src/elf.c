#include "elf.h"

#include <stdio.h>
#include <string.h>

static const char elf_magic_valid[] = "\x7f" "ELF";

#define ELF_CLASS_32 0x01
#define ELF_CLASS_64 0x02

#define ELF_VERSION_CURRENT 0x01

#define ELF_OSABI_SYSV     0x00
#define ELF_OSABI_HPUX     0x01
#define ELF_OSABI_NETBSD   0x02
#define ELF_OSABI_LINUX    0x03
#define ELF_OSABI_SOLARIS  0x06
#define ELF_OSABI_FREEBSD  0x09
#define ELF_OSABI_OPENBSD  0x0C

#define ELF_TYPE_NONE  0x0000
#define ELF_TYPE_REL   0x0001
#define ELF_TYPE_EXEC  0x0002
#define ELF_TYPE_DYN   0x0003
#define ELF_TYPE_CORE  0x0004

bool
is_elf_magic_valid(elf_magic *m)
{
  if (!m)
    return false;

  return memcmp(m->bytes, elf_magic_valid, sizeof(m->bytes)) == 0;
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
is_elf_version_valid(elf_header *m)
{
  if (!m)
    return false;

  return m->version == ELF_VERSION_CURRENT;
}

const char *
get_elf_version_str(elf_header *m)
{
  if (!m)
    return "unknown";

  if (m->version == ELF_VERSION_CURRENT)
    return "current 0x01";

  return "unrecognised";
}

bool
is_elf_abi_valid(elf_header *m)
{
  if (!m)
    return false;

  return strcmp(get_elf_abi_str(m), "unrecognised") != 0;
}

const char *
get_elf_abi_str(elf_header *m)
{
  if (!m)
    return "unknown";

  switch (m->abi)
  {
  case ELF_OSABI_SYSV:
    return "SysV";
  case ELF_OSABI_HPUX:
    return "HPUX";
  case ELF_OSABI_NETBSD:
    return "NetBSD";
  case ELF_OSABI_LINUX:
    return "Linux";
  case ELF_OSABI_SOLARIS:
    return "Solaris";
  case ELF_OSABI_FREEBSD:
    return "FreeBSD";
  case ELF_OSABI_OPENBSD:
    return "OpenBSD";
  default:
    return "unrecognised";
  }
}

bool
is_elf_abi_version_valid(elf_header *m)
{
  return m != NULL;
}

void
get_elf_abi_version_str(elf_header *m, char *buf, size_t buf_size)
{
  if (!buf || buf_size == 0)
    return;

  if (!m)
  {
    snprintf(buf, buf_size, "unknown");
    return;
  }

  snprintf(buf, buf_size, "%u", m->abi_version);
}

bool
is_elf_type_valid(elf_header *m)
{
  if (!m)
    return false;

  return strcmp(get_elf_type_str(m), "unrecognised") != 0;
}

const char *
get_elf_type_str(elf_header *m)
{
  if (!m)
    return "unknown";

  switch (m->type)
  {
  case ELF_TYPE_NONE:
    return "none";
  case ELF_TYPE_REL:
    return "relocatable";
  case ELF_TYPE_EXEC:
    return "executable";
  case ELF_TYPE_DYN:
    return "shared object";
  case ELF_TYPE_CORE:
    return "core";
  default:
    return "unrecognised";
  }
}

