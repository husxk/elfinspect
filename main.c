#include <stdbool.h>
#include <stdio.h>

#include "elf.h"
#include "elf_header_log.h"
#include "elf_program_header_log.h"
#include "elf_section_header_log.h"
#include "file.h"

static void usage(const char *prog)
{
  fprintf(stderr, "usage: %s <path>\n", prog);
}

static bool validate(int argc, char *argv[])
{
  (void)argv;
  return argc == 2;
}

static void
elf_inspect(file_t *f)
{
  elf_t *elf = NULL;

  if (!elf_create(&elf, f))
    return;

  if (!elf_parse(elf))
  {
    elf_destroy(elf);
    return;
  }

  log_elf_header(elf_ehdr(elf));
  log_elf_program_headers(elf_ehdr(elf), elf_program_headers(elf));
  log_elf_section_headers(elf_ehdr(elf), elf_section_headers(elf));

  elf_destroy(elf);
}

int main(int argc, char *argv[])
{
  if (!validate(argc, argv))
  {
    usage(argv[0]);
    return 1;
  }

  file_t *f = NULL;
  if (!file_open(&f, argv[1]))
    return 1;

  elf_inspect(f);

  if (!file_close(f))
    return 1;

  return 0;
}
