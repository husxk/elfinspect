#include "elf.h"

#include <stdio.h>
#include <string.h>

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

#define ELF_MACHINE_NONE     0
#define ELF_MACHINE_M32      1
#define ELF_MACHINE_SPARC    2
#define ELF_MACHINE_386      3
#define ELF_MACHINE_68K      4
#define ELF_MACHINE_88K      5
#define ELF_MACHINE_860      7
#define ELF_MACHINE_MIPS     8
#define ELF_MACHINE_PARISC   15
#define ELF_MACHINE_SPARC32P 18
#define ELF_MACHINE_PPC      20
#define ELF_MACHINE_PPC64    21
#define ELF_MACHINE_S390     22
#define ELF_MACHINE_ARM      40
#define ELF_MACHINE_SH       42
#define ELF_MACHINE_SPARCV9  43
#define ELF_MACHINE_IA_64    50
#define ELF_MACHINE_X86_64   62
#define ELF_MACHINE_AARCH64  183
#define ELF_MACHINE_RISCV    243

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

bool
is_elf_machine_valid(elf_header *m)
{
  if (!m)
    return false;

  return strcmp(get_elf_machine_str(m), "unrecognised") != 0;
}

const char *
get_elf_machine_str(elf_header *m)
{
  if (!m)
    return "unknown";

  switch (m->machine)
  {
  case ELF_MACHINE_NONE:
    return "none";
  case ELF_MACHINE_M32:
    return "AT&T WE 32100";
  case ELF_MACHINE_SPARC:
    return "Sparc";
  case ELF_MACHINE_386:
    return "x86";
  case ELF_MACHINE_68K:
    return "Motorola 68000";
  case ELF_MACHINE_88K:
    return "Motorola 88000";
  case ELF_MACHINE_860:
    return "Intel 80860";
  case ELF_MACHINE_MIPS:
    return "MIPS";
  case ELF_MACHINE_PARISC:
    return "HP PA-RISC";
  case ELF_MACHINE_SPARC32P:
    return "Sparc v8+";
  case ELF_MACHINE_PPC:
    return "PowerPC";
  case ELF_MACHINE_PPC64:
    return "PowerPC64";
  case ELF_MACHINE_S390:
    return "IBM S/390";
  case ELF_MACHINE_ARM:
    return "ARM";
  case ELF_MACHINE_SH:
    return "Renesas SH";
  case ELF_MACHINE_SPARCV9:
    return "Sparc v9";
  case ELF_MACHINE_IA_64:
    return "Intel IA-64";
  case ELF_MACHINE_X86_64:
    return "x86-64";
  case ELF_MACHINE_AARCH64:
    return "AArch64";
  case ELF_MACHINE_RISCV:
    return "RISC-V";
  default:
    return "unrecognised";
  }
}

bool
is_elf_header_version_valid(elf_header *m)
{
  if (!m)
    return false;

  return m->header_version == ELF_VERSION_CURRENT;
}

const char *
get_elf_header_version_str(elf_header *m)
{
  if (!m)
    return "unknown";

  if (m->header_version == ELF_VERSION_CURRENT)
    return "current 0x01";

  return "unrecognised";
}
