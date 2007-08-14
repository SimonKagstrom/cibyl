#include <cibyl-fileops.h>
#include <java/io.h>
#include <java/lang.h>

static int close(FILE *fp)
{
  NOPH_OutputStream_file_t *p = (NOPH_OutputStream_file_t *)fp->priv;

  NOPH_OutputStream_close(p->os);

  return 0;
}

static size_t write(FILE *fp, const void *ptr, size_t in_size)
{
  NOPH_OutputStream_file_t *p = (NOPH_OutputStream_file_t *)fp->priv;
  char *s;
  int n = 0;

  for ( s = (char *)ptr; s < (char*)ptr + in_size; s++, n++ )
    NOPH_OutputStream_write(p->os, *s);

  return n;
}

static int flush(FILE* fp)
{
  NOPH_OutputStream_file_t *p = (NOPH_OutputStream_file_t *)fp->priv;

  NOPH_OutputStream_flush(p->os);

  return 0;
}

/* The fops structure for resource files */
cibyl_fops_t NOPH_OutputStream_fops =
{
  .uri = NULL,
  .priv_data_size = sizeof(NOPH_OutputStream_file_t),
  .open = NULL,
  .close = close,
  .read = NULL,
  .write = write,
  .seek = NULL,
  .tell = NULL,
  .eof = NULL,
  .flush = flush,
};

FILE *NOPH_OutputStream_createFILE(NOPH_OutputStream_t os)
{
  FILE *out;
  NOPH_OutputStream_file_t *p;

  /* Get a new FILE object */
  out = cibyl_file_alloc(&NOPH_OutputStream_fops);
  p = (NOPH_OutputStream_file_t*)out->priv;

  p->os = os;

  return out;
}
