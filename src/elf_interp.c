#include "elf_interp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
elf_interp_destroy(char *interp)
{
  free(interp);
}

static const elf_program_header *
find_interp_program_header(const elf_program_header_table *phdrs)
{
  if (!phdrs)
    return NULL;

  for (size_t i = 0; i < phdrs->count; i++)
  {
    if (phdrs->entries[i].type == ELF_PT_INTERP)
      return &phdrs->entries[i];
  }

  return NULL;
}

bool
parse_elf_interp(file_t *f, const elf_header *eh, const elf_program_header_table *phdrs,
                 char **out)
{
  const elf_program_header *interp_ph;
  char *buf;
  size_t len;

  if (!out)
  {
    fprintf(stderr, "parse_elf_interp: output pointer is null\n");
    return false;
  }

  *out = NULL;

  if (!f || !eh)
  {
    fprintf(stderr, "parse_elf_interp: invalid argument\n");
    return false;
  }

  interp_ph = find_interp_program_header(phdrs);
  if (!interp_ph)
    return true;

  if (interp_ph->filesz == 0)
  {
    fprintf(stderr, "parse_elf_interp: PT_INTERP filesz is zero\n");
    return false;
  }

  if (interp_ph->filesz > SIZE_MAX)
  {
    fprintf(stderr, "parse_elf_interp: PT_INTERP filesz too large\n");
    return false;
  }

  len = (size_t)interp_ph->filesz;

  buf = malloc(len + 1);
  if (!buf)
  {
    fprintf(stderr, "parse_elf_interp: out of memory\n");
    return false;
  }

  if (!file_seek(f, interp_ph->offset))
  {
    free(buf);
    return false;
  }

  if (file_read(f, buf, len, len) != (int)len)
  {
    fprintf(stderr, "parse_elf_interp: failed to read interpreter path\n");
    free(buf);
    return false;
  }

  buf[len] = '\0';

  if (memchr(buf, '\0', len) == NULL)
  {
    fprintf(stderr, "parse_elf_interp: interpreter path is not NUL-terminated\n");
    free(buf);
    return false;
  }

  *out = buf;
  return true;
}
