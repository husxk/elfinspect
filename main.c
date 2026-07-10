#include <stdbool.h>
#include <stdio.h>

#include "elf.h"
#include "elf_header_parser.h"
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
  elf_header header = {0};

  if (!parse_elf_header(f, &header))
    return;
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
