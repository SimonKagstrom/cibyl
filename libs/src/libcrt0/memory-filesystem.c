#include <cibyl-fileops.h>
#include <cibyl-memoryfs.h>

typedef struct
{
  void *data;
  size_t data_size;
  off_t fp;
  int allocate;
} memory_file_t;

static int seek(FILE *p, long offset, int whence)
{
  memory_file_t *p = (memory_file_t*)fp->priv;
  int old = p->fp;

  if (whence == SEEK_SET)
    p->fp = offset;
  else if (whence == SEEK_CUR)
    p->fp = p->fp + offset;
  else if (whence == SEEK_END)
    p->fp = p->data_size + offset;
  else
    return -1;

  if (p->fp < 0)
    {
      p->fp = old;
      return -1;
    }

  if (p->fp >= p->data_size)
    p->fp = p->data_size;

  return 0;
}

static int tell(FILE *p)
{
  memory_file_t *p = (memory_file_t*)fp->priv;
  return p->fp;
}

static size_t read(FILE *p, void *ptr, size_t in_size)
{
  memory_file_t *p = (memory_file_t*)fp->priv;
  size_t size = min(in_size, p->data_size - p->fp);

  if (size >= 0)
    memcpy( ptr, p->data + p->fp, size );
  p->fp = p->fp + size;

  return size;
}

static size_t write(FILE *fp, void *ptr, size_t in_size)
{
  memory_file_t *p = (memory_file_t*)fp->priv;
  size_t new_size = max(p->fp + in_size, p->data_size);

  if (new_size > p->data_size)
    {
      p->data = realloc(p->data, new_size + 4096);
      if (!p->data)
        return -1;
    }

  if (in_size >= 0)
    memcpy( p->data + p->fp, ptr, in_size );
  p->fp = p->fp + in_size;

  return cnt;
}

static void close(FILE *fp)
{
  memory_file_t *p = (memory_file_t*)fp->priv;

  /* If this is allocated data, free it on closing */
  if (p->allocate)
    free(p->data);
}

static int eof(FILE *fp)
{
  memory_file_t *p = (memory_file_t*)fp->priv;

  return p->fp == p->data_size - 1;
}

static cibyl_fops_t memory_fops =
{
  .uri = NULL,  /* Not applicable */
  .priv_data_size = sizeof(mem_file_t),
  .open = NULL, /* Not applicable */
  .close = close,
  .read = read,
  .write = write,
  .seek = seek,
  .tell = tell,
  .eof = eof,
};

FILE *NOPH_memoryFileOpen(void *ptr, size_t size, int allocate)
{
  FILE *out;
  memory_file_t *p;

  /* Get a new FILE object */
  out = cibyl_file_alloc(&memory_fops);
  p = (memory_file_t*)out->priv;

  /* Fill in the private part of the FILE object */
  p->fp = 0;
  p->allocate = allocate;
  if (ptr == NULL)
    {
      p->data_size = size;
      p->data = malloc(p->data_size);
      p->allocate = 1;
      if (!p->data)
        {
          cibyl_file_free(out);
          /* FIXME: throw an OOM exception here */
          return NULL;
        }
    }
  else
    {
      p->data_size = size;
      p->data = ptr;
    }

  return out;
}
