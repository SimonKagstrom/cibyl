/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      utils.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Some utility fns
 *
 * $Id:$
 *
 ********************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <utils.h>

void *xcalloc(size_t nmemb, size_t size)
{
  void *out = calloc(nmemb, size);
  if (!out)
    {
      perror("calloc");
      exit(1);
    }

  return out;
}

void *xrealloc(void *ptr, size_t size)
{
  void *out = realloc(ptr, size);
  if (!out)
    {
      perror("realloc");
      exit(1);
    }

  return out;
}

file_chunk_t *read_file(const char *filename)
{
  file_chunk_t *out;
  struct stat buf;
  FILE *f;

  if (lstat(filename, &buf) < 0)
    {
      fprintf(stderr, "Cannot open file %s\n", filename);
      exit(1);
      return NULL;
    }

  out = (file_chunk_t*)xcalloc(sizeof(file_chunk_t), 1);

  out->size = buf.st_size;
  out->data = malloc(out->size);
  f = fopen(filename, "r");
  assert(fread(out->data, 1, out->size, f) == out->size);
  fclose(f);

  return out;
}

FILE *open_file_in_dir(const char *dir, const char *filename, const char *mode)
{
  int len = strlen(dir) + strlen(filename) + 4;
  FILE *fp;
  char *buf;

  buf = (char*)xcalloc(len, 1);

  snprintf(buf, len, "%s/%s", dir, filename);
  fp = fopen(buf, mode);
  free(buf);
  if (!fp)
    {
      fprintf(stderr, "Cannot open file %s/%s\n", dir, filename);
      exit(1);
    }

  return fp;
}


/* From tobin */
static uint32_t swap32(uint32_t x)
{
  uint32_t out;


  out = ( (x & 0xffull) << 24 ) |
    ( ((x & 0xff00ull) >> 8) << 16) |
    ( ((x & 0xff0000ull) >> 16) << 8) |
    ( ((x & 0xff000000ull) >> 24) << 0);

      return out;
}

static uint64_t swap64(uint64_t x)
{
  uint64_t out;

  out = ( ((x & 0xffull) << 56 ) |
          ( ((x & 0xff00ull) >> 8) << 48 ) |
          ( ((x & 0xff0000ull) >> 16) << 40 ) |
          ( ((x & 0xff000000ull) >> 24) << 32 ) |
          ( ((x & 0xff00000000ull) >> 32) << 24 ) |
          ( ((x & 0xff0000000000ull) >> 40)  << 16 ) |
          ( ((x & 0xff000000000000ull) >> 48) << 8 ) |
          ( ((x & 0xff00000000000000ull) >> 56) << 0 ) );

return out;
}

unsigned long be_to_host(unsigned long in)
{
  if (sizeof(unsigned long) == 4)
    return (unsigned long)swap32((uint32_t)in);
  else if (sizeof(unsigned long) == 8)
    return (unsigned long)swap64((uint64_t)in);

  fprintf(stderr, "ERROR: unsigned long must be 4 or 8 bytes!\n");
  exit(1);
}
