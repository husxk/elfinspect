#include "elf_interp_log.h"

#include <stdio.h>

void
log_elf_interp(const char *interp)
{
  if (!interp || interp[0] == '\0')
    return;

  printf("%s\n", interp);
}
