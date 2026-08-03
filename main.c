#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

#include "elf.h"
#include "elf_header_log.h"
#include "elf_program_header_log.h"
#include "elf_section_header_log.h"
#include "elf_symbol_log.h"
#include "elf_dynamic_log.h"
#include "elf_note_log.h"
#include "file.h"

typedef struct
{
  bool show_file_header;
  bool show_program_headers;
  bool show_section_headers;
  bool show_symbols;
  bool show_dynamic;
  bool show_needed;
  bool show_notes;
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
          "  -s, --symbols           Symbol tables\n"
          "  -d, --dynamic           Dynamic section\n"
          "  -N, --needed            Needed shared libraries (DT_NEEDED)\n"
          "  -n, --notes             Note sections\n"
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
      {"symbols", no_argument, NULL, 's'},
      {"dynamic", no_argument, NULL, 'd'},
      {"needed", no_argument, NULL, 'N'},
      {"notes", no_argument, NULL, 'n'},
      {"help", no_argument, NULL, 'H'},
      {NULL, 0, NULL, 0},
  };

  bool any_display = false;

  opts->show_file_header = false;
  opts->show_program_headers = false;
  opts->show_section_headers = false;
  opts->show_symbols = false;
  opts->show_dynamic = false;
  opts->show_needed = false;
  opts->show_notes = false;
  *path_out = NULL;

  opterr = 0;

  for (;;)
  {
    int c = getopt_long(argc, argv, "hlSsdNn", long_options, NULL);

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

    case 's':
      opts->show_symbols = true;
      any_display = true;
      break;

    case 'd':
      opts->show_dynamic = true;
      any_display = true;
      break;

    case 'N':
      opts->show_needed = true;
      any_display = true;
      break;

    case 'n':
      opts->show_notes = true;
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

  if (opts->show_symbols)
    log_elf_symbols(elf_ehdr(elf), elf_section_headers(elf), elf_shstrtab(elf),
                    elf_symbol_tables(elf));

  if (opts->show_dynamic && elf_dynamic_section(elf))
    log_elf_dynamic(elf_ehdr(elf), elf_section_headers(elf), elf_shstrtab(elf),
                    elf_dynamic_section(elf));

  if (opts->show_needed && elf_dynamic_section(elf))
    log_elf_needed_libraries(elf_dynamic_section(elf));

  if (opts->show_notes && elf_notes(elf))
    log_elf_notes(elf_ehdr(elf), elf_section_headers(elf), elf_shstrtab(elf), elf_notes(elf));

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
