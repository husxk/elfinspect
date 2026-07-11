#include "elf_endian.h"

#include <endian.h>
#include <stdio.h>
#include <string.h>

static bool
is_endian_valid(uint8_t endian)
{
  return endian == ELF_ENDIAN_LITTLE || endian == ELF_ENDIAN_BIG;
}

bool
is_elf_endian_valid(const elf_header *header)
{
  if (!header)
    return false;

  return is_endian_valid(header->endian);
}

const char *
get_elf_endian_str(const elf_header *header)
{
  if (!header)
    return "unknown";

  if (header->endian == ELF_ENDIAN_LITTLE)
    return "little";
  if (header->endian == ELF_ENDIAN_BIG)
    return "big";

  return "unrecognised";
}

static uint16_t
to_host_u16(uint16_t v, uint8_t endian)
{
  if (endian == ELF_ENDIAN_LITTLE)
    return le16toh(v);

  return be16toh(v);
}

static uint32_t
to_host_u32(uint32_t v, uint8_t endian)
{
  if (endian == ELF_ENDIAN_LITTLE)
    return le32toh(v);

  return be32toh(v);
}

static uint64_t
to_host_u64(uint64_t v, uint8_t endian)
{
  if (endian == ELF_ENDIAN_LITTLE)
    return le64toh(v);

  return be64toh(v);
}

static uint16_t
to_host_u16_raw(const void *raw, uint8_t endian)
{
  uint16_t v;

  memcpy(&v, raw, sizeof(v));
  return to_host_u16(v, endian);
}

static uint32_t
to_host_u32_raw(const void *raw, uint8_t endian)
{
  uint32_t v;

  memcpy(&v, raw, sizeof(v));
  return to_host_u32(v, endian);
}

static uint64_t
to_host_u64_raw(const void *raw, uint8_t endian)
{
  uint64_t v;

  memcpy(&v, raw, sizeof(v));
  return to_host_u64(v, endian);
}

static bool
elf_read_raw(file_t *f, void *buf, size_t size)
{
  int n = file_read(f, buf, size, size);

  return n == (int)size;
}

static bool
elf_read_multibyte(file_t *f, const elf_header *header, void *raw, size_t raw_size)
{
  if (!header)
    return false;

  if (!is_endian_valid(header->endian))
  {
    fprintf(stderr, "elf_read: invalid endian 0x%02x\n", header->endian);
    return false;
  }

  return elf_read_raw(f, raw, raw_size);
}

bool
elf_read_u16(file_t *f, elf_header *header, uint16_t *out)
{
  uint8_t raw[2];

  if (!out || !elf_read_multibyte(f, header, raw, sizeof(raw)))
    return false;

  *out = to_host_u16_raw(raw, header->endian);
  return true;
}

bool
elf_read_u32(file_t *f, elf_header *header, uint32_t *out)
{
  uint8_t raw[4];

  if (!out || !elf_read_multibyte(f, header, raw, sizeof(raw)))
    return false;

  *out = to_host_u32_raw(raw, header->endian);
  return true;
}

bool
elf_read_u64(file_t *f, elf_header *header, uint64_t *out)
{
  uint8_t raw[8];

  if (!out || !elf_read_multibyte(f, header, raw, sizeof(raw)))
    return false;

  *out = to_host_u64_raw(raw, header->endian);
  return true;
}
