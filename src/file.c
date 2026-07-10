#include "file.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct file
{
  FILE *fp;
};

bool file_open(file_t **out, const char *path)
{
  if (!out)
  {
    fprintf(stderr, "file_open: output pointer is null\n");
    return false;
  }

  if (!path)
  {
    fprintf(stderr, "file_open: path is null\n");
    return false;
  }

  file_t *f = calloc(1, sizeof(*f));
  if (!f)
  {
    fprintf(stderr, "file_open: out of memory\n");
    return false;
  }

  f->fp = fopen(path, "rb");
  if (!f->fp)
  {
    fprintf(stderr, "file_open: failed to open '%s': %s\n", path, strerror(errno));
    free(f);
    return false;
  }

  *out = f;
  return true;
}

bool file_close(file_t *f)
{
  if (!f)
  {
    fprintf(stderr, "file_close: file is null\n");
    return false;
  }

  bool ok = true;
  if (f->fp && fclose(f->fp) != 0)
  {
    fprintf(stderr, "file_close: fclose failed: %s\n", strerror(errno));
    ok = false;
  }

  free(f);
  return ok;
}

int file_read(file_t *f, void *buf, size_t buf_size, size_t count)
{
  if (!f || !f->fp)
  {
    fprintf(stderr, "file_read: file is not open\n");
    return -1;
  }

  if (!buf)
  {
    fprintf(stderr, "file_read: buffer is null\n");
    return -1;
  }

  if (count > buf_size)
  {
    fprintf(stderr, "file_read: count (%zu) exceeds buffer size (%zu)\n", count, buf_size);
    return -1;
  }

  if (count == 0)
    return 0;

  size_t n = fread(buf, 1, count, f->fp);
  if (n < count && ferror(f->fp))
  {
    fprintf(stderr, "file_read: read failed: %s\n", strerror(errno));
    return -1;
  }

  return (int)n;
}
