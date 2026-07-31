#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

#include "elf.h"
#include "elf_header_log.h"
#include "elf_program_header_log.h"
#include "elf_section_header_log.h"
#include "file.h"

typedef struct
{
  bool show_file_header;
  bool show_program_headers;
  bool show_section_headers;
} inspect_options;

typedef enum
{
  PARSE_OK = 0,
  PARSE_HELP,
  PARSE_ERROR,
} parse_result;

static void
usage(const char *prog)
{
  fprintf(stderr,
          "usage: %s [options] <path>\n"
          "\n"
          "Display ELF information (default: all sections below).\n"
          "\n"
          "Options:\n"
          "  -h, --file-header       ELF file header\n"
          "  -l, --program-headers   Program headers\n"
          "  -S, --section-headers   Section headers\n"
          "      --help              Show this help\n",
          prog);
}

static parse_result
parse_options(int argc, char *argv[], inspect_options *opts, const char **path_out)
{
  static const struct option long_options[] = {
      {"file-header", no_argument, NULL, 'h'},
      {"program-headers", no_argument, NULL, 'l'},
      {"section-headers", no_argument, NULL, 'S'},
      {"help", no_argument, NULL, 'H'},
      {NULL, 0, NULL, 0},
  };

  bool any_display = false;

  opts->show_file_header = false;
  opts->show_program_headers = false;
  opts->show_section_headers = false;
  *path_out = NULL;

  opterr = 0;

  for (;;)
  {
    int c = getopt_long(argc, argv, "hlS", long_options, NULL);

    if (c == -1)
      break;

    switch (c)
    {
    case 'h':
      opts->show_file_header = true;
      any_display = true;
      break;

    case 'l':
      opts->show_program_headers = true;
      any_display = true;
      break;

    case 'S':
      opts->show_section_headers = true;
      any_display = true;
      break;

    case 'H':
      usage(argv[0]);
      return PARSE_HELP;

    case '?':
      if (optopt != 0)
        fprintf(stderr, "%s: invalid option -- '%c'\n", argv[0], optopt);
      else
        fprintf(stderr, "%s: invalid option '%s'\n", argv[0], argv[optind - 1]);
      usage(argv[0]);
      return PARSE_ERROR;

    default:
      usage(argv[0]);
      return PARSE_ERROR;
    }
  }

  if (!any_display)
  {
    opts->show_file_header = true;
    opts->show_program_headers = true;
    opts->show_section_headers = true;
  }

  if (optind >= argc)
  {
    fprintf(stderr, "%s: missing path argument\n", argv[0]);
    usage(argv[0]);
    return PARSE_ERROR;
  }

  if (optind + 1 < argc)
  {
    fprintf(stderr, "%s: extra operand '%s'\n", argv[0], argv[optind + 1]);
    usage(argv[0]);
    return PARSE_ERROR;
  }

  *path_out = argv[optind];
  return PARSE_OK;
}

static void
elf_inspect(file_t *f, const inspect_options *opts)
{
  elf_t *elf = NULL;

  if (!elf_create(&elf, f))
    return;

  if (!elf_parse(elf))
  {
    elf_destroy(elf);
    return;
  }

  if (opts->show_file_header)
    log_elf_header(elf_ehdr(elf));

  if (opts->show_program_headers)
    log_elf_program_headers(elf_ehdr(elf), elf_program_headers(elf));

  if (opts->show_section_headers)
    log_elf_section_headers(elf_ehdr(elf), elf_section_headers(elf), elf_shstrtab(elf));

  elf_destroy(elf);
}

int main(int argc, char *argv[])
{
  inspect_options opts;
  const char *path = NULL;
  parse_result pr;

  pr = parse_options(argc, argv, &opts, &path);
  if (pr == PARSE_HELP)
    return 0;
  if (pr != PARSE_OK)
    return 1;

  file_t *f = NULL;
  if (!file_open(&f, path))
    return 1;

  elf_inspect(f, &opts);

  if (!file_close(f))
    return 1;

  return 0;
}
