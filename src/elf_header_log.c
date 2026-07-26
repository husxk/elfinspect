#include "elf_header_log.h"

#include <ctype.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>

#include "elf_endian.h"
#include "elf_file_header.h"

#define LABEL_WIDTH 36

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

static void
log_field(const char *label, const char *fmt, ...)
{
  char value[128];
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(value, sizeof(value), fmt, ap);
  va_end(ap);

  printf("  %-*s %s\n", LABEL_WIDTH, label, value);
}

static const char *
elf_class_str(const elf_header *header)
{
  if (!header)
    return "unknown";

  if (header->class == ELF_CLASS_32)
    return "ELF32";
  if (header->class == ELF_CLASS_64)
    return "ELF64";

  return "unrecognised";
}

static const char *
elf_endian_str(const elf_header *header)
{
  if (!header)
    return "unknown";

  if (header->endian == ELF_ENDIAN_LITTLE)
    return "little";
  if (header->endian == ELF_ENDIAN_BIG)
    return "big";

  return "unrecognised";
}

static const char *
elf_ident_version_str(const elf_header *header)
{
  if (!header)
    return "unknown";

  if (header->version == ELF_VERSION_CURRENT)
    return "current 0x01";

  return "unrecognised";
}

static const char *
elf_abi_str(const elf_header *header)
{
  if (!header)
    return "unknown";

  switch (header->abi)
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

static void
elf_abi_version_str(const elf_header *header, char *buf, size_t buf_size)
{
  if (!buf || buf_size == 0)
    return;

  if (!header)
  {
    snprintf(buf, buf_size, "unknown");
    return;
  }

  snprintf(buf, buf_size, "%u", header->abi_version);
}

static const char *
elf_type_str(const elf_header *header)
{
  if (!header)
    return "unknown";

  switch (header->type)
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

static const char *
elf_machine_str(const elf_header *header)
{
  if (!header)
    return "unknown";

  switch (header->machine)
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

static const char *
elf_header_version_str(const elf_header *header)
{
  if (!header)
    return "unknown";

  if (header->header_version == ELF_VERSION_CURRENT)
    return "current 0x01";

  return "unrecognised";
}

static void
log_magic(const elf_header *header)
{
  char ascii_str[sizeof(header->magic.bytes) + 1];
  char hex_str[sizeof(header->magic.bytes) * 3];
  char magic_str[sizeof(hex_str) + 4 + sizeof(ascii_str)];
  size_t pos = 0;

  for (size_t i = 0; i < sizeof(header->magic.bytes); i++)
  {
    ascii_str[i] = (char)(isprint(header->magic.bytes[i]) ? header->magic.bytes[i] : '.');

    if (i > 0)
      hex_str[pos++] = ' ';

    pos += (size_t)snprintf(hex_str + pos, sizeof(hex_str) - pos,
                            "%02x", header->magic.bytes[i]);
  }

  ascii_str[sizeof(header->magic.bytes)] = '\0';

  snprintf(magic_str, sizeof(magic_str), "%s   %s", hex_str, ascii_str);
  log_field("Magic:", "%s", magic_str);
}

void
log_elf_header(const elf_header *header)
{
  char abi_version_str[16];

  if (!header)
    return;

  printf("ELF Header\n");

  log_magic(header);
  log_field("Class:", "%s", elf_class_str(header));
  log_field("Endian:", "%s", elf_endian_str(header));
  log_field("Version:", "%s", elf_ident_version_str(header));
  log_field("OS/ABI:", "%s", elf_abi_str(header));

  elf_abi_version_str(header, abi_version_str, sizeof(abi_version_str));
  log_field("ABI version:", "%s", abi_version_str);

  log_field("Type:", "%s", elf_type_str(header));
  log_field("Machine:", "%s", elf_machine_str(header));
  log_field("Header version:", "%s", elf_header_version_str(header));
  log_field("Entry point address:", "0x%0*" PRIx64,
            (int)elf_word_size(header) * 2, header->entry);
  log_field("Program header table offset:", "%" PRIu64 " (0x%" PRIx64 ")",
            header->program_header_offset, header->program_header_offset);
  log_field("Section header table offset:", "%" PRIu64 " (0x%" PRIx64 ")",
            header->section_header_offset, header->section_header_offset);
  log_field("Flags:", "0x%" PRIx32, header->flags);
  log_field("ELF header size:", "%u", header->elf_header_size);
  log_field("Program header table entry size:", "%u", header->ph_table_entry_size);
  log_field("Program header table entry count:", "%u", header->ph_table_entry_count);
  log_field("Section header table entry size:", "%u", header->sh_table_entry_size);
  log_field("Section header table entry count:", "%u", header->sh_table_entry_count);
  log_field("Section header string table index:", "%u", header->sh_string_table_index);
}
