#include <string.h>
#include <stdlib.h>

#include <cibyl-fileops.h>
#include <cibyl-memoryfs.h>

#define min(x,y) ( (x) < (y) ? (x) : (y) )
#define max(x,y) ( (x) > (y) ? (x) : (y) )

typedef struct
{
  void *data;
  size_t data_size;
  long fp;
  int allocate;
} memory_file_t;

static int seek(FILE *fp, long offset, int whence)
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

static long tell(FILE *fp)
{
  memory_file_t *p = (memory_file_t*)fp->priv;
  return p->fp;
}

static size_t read(FILE *fp, void *ptr, size_t in_size)
{
  memory_file_t *p = (memory_file_t*)fp->priv;
  size_t size = min(in_size, p->data_size - p->fp);

  if (size >= 0)
    memcpy( ptr, p->data + p->fp, size );
  p->fp = p->fp + size;

  return size;
}

static size_t write(FILE *fp, const void *ptr, size_t in_size)
{
  memory_file_t *p = (memory_file_t*)fp->priv;
  size_t new_size = max(p->fp + in_size, p->data_size);
  long old = p->fp;

  if (new_size > p->data_size)
    {
      p->data = realloc(p->data, new_size + 4096);
      if (!p->data)
        return -1;
    }

  if (in_size >= 0)
    memcpy( p->data + p->fp, ptr, in_size );
  p->fp = p->fp + in_size;

  return p->fp - old;
}

static int close(FILE *fp)
{
  memory_file_t *p = (memory_file_t*)fp->priv;

  /* If this is allocated data, free it on closing */
  if (p->allocate)
    free(p->data);

  return 0;
}

static int eof(FILE *fp)
{
  memory_file_t *p = (memory_file_t*)fp->priv;

  return p->fp == p->data_size - 1;
}

static cibyl_fops_t memory_fops =
{
  .uri = NULL,  /* Not applicable */
  .priv_data_size = sizeof(memory_file_t),
  .open = NULL, /* Not applicable */
  .close = close,
  .read = read,
  .write = write,
  .seek = seek,
  .tell = tell,
  .eof = eof,
};

void *NOPH_MemoryFile_getDataPtr(FILE *fp)
{
  memory_file_t *p = (memory_file_t*)fp->priv;

  return p->data;
}

FILE *NOPH_MemoryFile_open(void *ptr, size_t size, int allocate)
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

FILE *NOPH_MemoryFile_openIndirect(const char *name, const char *mode)
{
  FILE *tmp;
  FILE *out;
  void *data = NULL;
  size_t size = 0;
  size_t n = 0;

  tmp = fopen(name, mode);
  if (!tmp)
    return NULL;

  /* Read in all of the file */
  do
    {
      size += 4096;
      data = realloc(data, size);
      if (!data)
        {
          fclose(tmp);
          return NULL;
        }
      n = fread(data, 1, 4096, tmp);
    } while(n == 4096);
  fclose(tmp);

  /* Open the memory file */
  out = NOPH_MemoryFile_open(data, size, 1);
  if (!out)
    free(data); /* Should never happen, but OK */
  return out;
}
