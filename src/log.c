#include "log.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static void
hexdump_impl(const unsigned char *data, size_t len, bool quiet)
{
  if (!quiet)
    fprintf(stderr, "hexdump len %zu\n", len);

  char dumpdata[16 * 3];
  char dumpdata2[17];

  memset(dumpdata, '\0', sizeof(dumpdata));
  memset(dumpdata2, '\0', sizeof(dumpdata2));

  const char hex_chars[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                            '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

  for (size_t p = 0; p < len; p++)
  {
    const int c = data[p];
    const int index = (int)(p % 16);
    const int offset = 3 * index;

    dumpdata[offset] = hex_chars[(c >> 4) & 0x0F];
    dumpdata[offset + 1] = hex_chars[c & 0x0F];
    dumpdata[offset + 2] = ' ';
    dumpdata2[index] = (char)(isprint(c) ? c : '.');

    if (index == 15 || p == len - 1)
    {
      dumpdata[16 * 3 - 1] = '\0';
      dumpdata2[16] = '\0';
      fprintf(stderr, "%-48s   %s\n", dumpdata, dumpdata2);

      memset(dumpdata, '\0', sizeof(dumpdata));
      memset(dumpdata2, '\0', sizeof(dumpdata2));
    }
  }
}

void
hexdump(const unsigned char *data, size_t len)
{
  hexdump_impl(data, len, false);
}

void
hexdump_quiet(const unsigned char *data, size_t len)
{
  hexdump_impl(data, len, true);
}

void
logit(const char *msg)
{
  if (msg)
    fprintf(stderr, "%s\n", msg);
}

void
logit_hexdump(const char *msg, const unsigned char *data, size_t len)
{
  logit(msg);
  hexdump_quiet(data, len);
}
