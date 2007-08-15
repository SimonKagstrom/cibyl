#include <cibyl-fileops.h>
#include <javax/microedition/lcdui/game.h>
#include <java/io.h>
#include <java/lang.h>

#define min(x,y) ( (x) < (y) ? (x) : (y) )

static int close(FILE *fp)
{
  NOPH_InputStream_file_t *p = (NOPH_InputStream_file_t *)fp->priv;

  NOPH_InputStream_close(p->is);

  return 0;
}


static int seek(FILE *fp, long offset, int whence)
{
  NOPH_InputStream_file_t *p = (NOPH_InputStream_file_t *)fp->priv;
  int skip = offset;
  int avail;
  int error;

  /* On input streams in general we assume all these can be
   * supported. Otherwise this function should be reimplemented in an
   * "inheriting" class
   */
  switch (whence)
    {
    case SEEK_SET:
      p->is_fp = 0;
      NOPH_InputStream_reset(p->is);
      break;
    case SEEK_END:
      p->is_fp = 0;
      avail = NOPH_InputStream_available(p->is);
      skip = avail - offset;
      NOPH_InputStream_reset(p->is);
      break;
    case SEEK_CUR:
      /* Do nothing (fallthrough) */
    default:
      break;
    }
  NOPH_try(NOPH_setter_exception_handler, (void*)&error)
    {
      p->is_fp += NOPH_InputStream_skip(p->is, skip);
    } NOPH_catch();
  if (error)
    return -1;

  return 0;
}

static long tell(FILE *fp)
{
  NOPH_InputStream_file_t *p = (NOPH_InputStream_file_t *)fp->priv;

  return p->is_fp;
}

static size_t read(FILE *fp, void *ptr, size_t in_size)
{
  NOPH_InputStream_file_t *p = (NOPH_InputStream_file_t *)fp->priv;
  long before = p->is_fp;

  /* Cached file reading - read into a temporary buffer */
  while (in_size > 0)
    {
      int n;
      int error;
      size_t size = min(in_size, 4096);

      /* Read the data into the buffer, potentially setting fp->eof */
      n = NOPH_InputStream_read_into(p->is, ptr, size, &fp->eof);
      p->is_fp += n;
      in_size -= n;
      ptr += n;
    }

  return p->is_fp - before;
}

static size_t write(FILE *fp, const void *ptr, size_t in_size)
{
  /* FIXME: Should we throw something? */
  return 0;
}


/* The fops structure for resource files */
cibyl_fops_t NOPH_InputStream_fops =
{
  .priv_data_size = sizeof(NOPH_InputStream_file_t),
  .open = NULL,
  .close = close,
  .read = read,
  .write = write,
  .seek = seek,
  .tell = tell,
};

FILE *NOPH_InputStream_createFILE(NOPH_InputStream_t is)
{
  FILE *out;
  NOPH_InputStream_file_t *p;

  /* Get a new FILE object */
  out = cibyl_file_alloc(&NOPH_InputStream_fops);
  p = (NOPH_InputStream_file_t*)out->priv;

  p->is = is;

  return out;
}
