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
#include <stdarg.h>

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

void *read_cpp(size_t *out_size, const char **defines, const char *fmt, ...)
{
  struct stat buf;
  const char *cpp;
  char path[2048];
  va_list ap;
  void *data;
  size_t size;
  FILE *f;
  int r, l;

  cpp = getenv("CIBYL_CPP");
  if (!cpp)
    cpp = "cpp";

  panic_if(strlen(cpp) > 2048,
           "Strlen too large in CPP pipe\n");

  /* pipe cpp, -C menas keep comments, -P means emit no line information */
  l = snprintf(path, 2048, "%s -C -P ", cpp);
  for (int i = 0; defines[i]; i++)
    {
      const char *def = defines[i];
      l += snprintf(path + l, 2048 - l, "%s ", def);
    }

  /* Create the command */
  assert ( fmt != NULL );
  va_start(ap, fmt);
  r = vsnprintf(path + l, 2048 - l, fmt, ap);
  va_end(ap);

  /* No file to CPP? */
  if (lstat(path + l, &buf) < 0)
    return NULL;

  f = popen(path, "r");
  panic_if(!f, "popen %s failed\n", path);
  data = xcalloc(buf.st_size * 4, 1);

  while ( (size = fread(data, 1, buf.st_size * 4, f)) != 0)
    {
      panic_if (size != 0 && size >= (size_t)(buf.st_size * 4),
                "Outbuffer of %s is too large: %u vs %ld\n",
                path, size, buf.st_size * 4);
    }
  fclose(f);

  *out_size = size;

  return data;
}

void *read_file(size_t *out_size, const char *fmt, ...)
{
  struct stat buf;
  char path[2048];
  va_list ap;
  void *data;
  size_t size;
  FILE *f;
  int r;

  /* Create the filename */
  assert ( fmt != NULL );
  va_start(ap, fmt);
  r = vsnprintf(path, 2048, fmt, ap);
  va_end(ap);

  if (lstat(path, &buf) < 0)
    return NULL;

  size = buf.st_size;
  data = xcalloc(size + 2, 1); /* NULL-terminate, if used as string */
  f = fopen(path, "r");
  if (!f)
    {
      free(data);
      return NULL;
    }
  if (fread(data, 1, size, f) != size)
    {
      free(data);
      data = NULL;
    }
  fclose(f);

  *out_size = size;

  return data;
}

  DIR *open_dir_fmt(const char *fmt, ...)
  {
    char path[2048];
    va_list ap;
    int r;

    /* Create the dirname */
    assert ( fmt != NULL );
    va_start(ap, fmt);
    r = vsnprintf(path, 2048, fmt, ap);
    va_end(ap);

    return opendir(path);
  }


  FILE *open_file_in_dir(const char *dir, const char *filename, const char *mode)
  {
    int len = strlen(dir) + strlen(filename) + 4;
    FILE *fp;
    char *buf;

    buf = (char*)xcalloc(len, 1);

    xsnprintf(buf, len, "%s/%s", dir, filename);
    fp = fopen(buf, mode);
    free(buf);
    panic_if(!fp, "Cannot open file %s/%s\n", dir, filename);

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

    panic("ERROR: unsigned long must be 4 or 8 bytes!\n");
  }
