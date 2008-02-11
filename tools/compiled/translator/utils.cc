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
      perror("lstat");
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
