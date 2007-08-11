#include <cibyl-fileops.h>
#include <java/lang/io.h>

typedef struct
{
  NOPH_InputStream_t is;
  off_t fp;
  int eof;
} resource_file_t;

static void exception_handler(NOPH_Exception_t ex, void *arg)
{
  int *p = (int*)arg;
  *p = 1;

  NOPH_delete(ex);
}

static int open(FILE *fp, const char *path,
                cibyl_fops_open_mode_t mode)
{
  NOPH_GameCanvas_t gc = NOPH_GameCanvas_get();
  resource_file_t *p = (resource_file_t *)fp->priv;
  int error;

  p->fp = 0;
  p->eof = 0;

  /* Try to open the resource stream. They are always read-only, so
   * just ignore the mode
   */
  NOPH_try(exception_handler, (void*)&error)
    {
      p->is = NOPH_Class_getResourceAsStream(gc, path);
    } NOPH_catch();
  if (error)
    return -1;

  return 0;
}

static void close(FILE *fp)
{
  resource_file_t *p = (resource_file_t *)fp->priv;

  NOPH_InputStream_close(p->is);
}


static int seek(FILE *p, long offset, int whence)
{
  resource_file_t *p = (resource_file_t *)fp->priv;
  int skip = offset;
  int avail;
  int error;

  /* On resource streams, all these can be supported */
  switch (whence)
    {
    case SEEK_SET:
      p->fp = 0;
      NOPH_InputStream_reset(p->is);
      break;
    case SEEK_END:
      p->fp = 0;
      avail = NOPH_InputStream_available(p->is);
      skip = avail - offset;
      NOPH_InputStream_reset(p->is);
      break;
    case SEEK_CUR:
      /* Do nothing (fallthrough) */
    default:
      break;
    }
  NOPH_try(exception_handler, (void*)&error)
    {
      p->fp += NOPH_InputStream_skip(p->is, skip);
    } NOPH_catch();
  if (error)
    return -1;

  return 0;
}

static int tell(FILE *p)
{
  resource_file_t *p = (resource_file_t *)fp->priv;

  return p->fp;
}

static size_t read(FILE *p, void *ptr, size_t in_size)
{
  resource_file_t *p = (resource_file_t *)fp->priv;

  /* Cached file reading - read into a temporary buffer */
  while (in_size > 0)
    {
      int n;
      int error;

      NOPH_try(exception_handler, (void*)&error)
        {
          size_t size = min(in_size, 8192);

          n = NOPH_InputStream_read_into(p->is, ptr, size);
          p->fp += n;
          in_size -= n;
          ptr += n;
        } NOPH_catch();
      if (error)
        {
          p->eof = 1;
          break;
        }
    }

  return size;
}

static size_t write(FILE *fp, void *ptr, size_t in_size)
{
  /* FIXME: Should we throw something? */
  return 0;
}

static int eof(FILE *fp)
{
  resource_file_t *p = (resource_file_t *)fp->priv;

  return p->eof;
}


/* The fops structure for resource files */
static cibyl_fops_t memory_fops =
{
  .uri = "resource://",
  .priv_data_size = sizeof(resource_file_t),
  .open = open,
  .close = close,
  .read = read,
  .write = write,
  .seek = seek,
  .tell = tell,
  .eof = eof,
};

static void __attribute__((constructor))register_fs(void)
{
  cibyl_register_fops(&resource_fops, 1);
}
