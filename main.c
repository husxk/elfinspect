#include <stdbool.h>
#include <stdio.h>

#include "elf.h"
#include "elf_header_log.h"
#include "elf_header_parser.h"
#include "elf_program_header.h"
#include "elf_program_header_log.h"
#include "elf_section_header.h"
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
  elf_header header = {0};

  if (!parse_elf_header(f, &header))
    return;

  log_elf_header(&header);

  elf_program_header_table *ph_table = NULL;
  if (!elf_program_header_table_create(header.ph_table_entry_count, &ph_table))
    return;

  if (!parse_elf_program_headers(f, &header, ph_table))
  {
    elf_program_header_table_destroy(ph_table);
    return;
  }

  log_elf_program_headers(&header, ph_table);

  elf_program_header_table_destroy(ph_table);

  elf_section_header_table *sh_table = NULL;
  if (!elf_section_header_table_create(header.sh_table_entry_count, &sh_table))
    return;

  if (!parse_elf_section_headers(f, &header, sh_table))
  {
    elf_section_header_table_destroy(sh_table);
    return;
  }

  log_elf_section_headers(&header, sh_table);

  elf_section_header_table_destroy(sh_table);
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
