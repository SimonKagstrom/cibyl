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


static void seek(FILE *fp, long offset)
{
  NOPH_InputStream_file_t *p = (NOPH_InputStream_file_t *)fp->priv;
  long skip = offset;
  int real_skip;

  if (offset < 0)
    {
      NOPH_InputStream_reset(p->is);
      skip = fp->vfptr;
    }
  real_skip = NOPH_InputStream_skip(p->is, skip); /* Might throw */

  if (real_skip != skip)
    NOPH_throw(NOPH_Exception_new());
}

static size_t read(FILE *fp, void *ptr, size_t in_size)
{
  NOPH_InputStream_file_t *p = (NOPH_InputStream_file_t *)fp->priv;
  size_t count = 0;

  /* Cached file reading - read into a temporary buffer */
  while (in_size > 0)
    {
      size_t size = min(in_size, 4096);
      int n;

      /* Read the data into the buffer, potentially setting fp->eof */
      n = NOPH_InputStream_read_into(p->is, ptr, size, &fp->eof);
      count += n;
      in_size -= n;
      ptr += n;
      if (fp->eof)
        break;
    }

  return count;
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
};

FILE *NOPH_InputStream_createFILE(NOPH_InputStream_t is)
{
  FILE *out;
  NOPH_InputStream_file_t *p;

  /* Get a new FILE object */
  out = cibyl_file_alloc(&NOPH_InputStream_fops);
  p = (NOPH_InputStream_file_t*)out->priv;

  p->is = is;
  out->file_size = NOPH_InputStream_available(is);

  return out;
}
