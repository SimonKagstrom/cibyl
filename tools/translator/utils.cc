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
  data = xcalloc(buf.st_size * 4 + 1, 1);

  while ( (size = fread(data, 1, buf.st_size * 4, f)) != 0)
    {
      panic_if (size != 0 && size >= (size_t)(buf.st_size * 4),
                "Outbuffer of %s is too large: %zd vs %ld\n",
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

int file_exists(const char *fmt, ...)
{
  struct stat buf;
  char path[2048];
  va_list ap;

  /* Create the filename */
  assert ( fmt != NULL );
  va_start(ap, fmt);
  vsnprintf(path, sizeof(path), fmt, ap);
  va_end(ap);

  if (lstat(path, &buf) < 0)
    return 0;

  return 1;
}

static void create_dir_structure(const char *dir)
{
  char *p, *dst;
  const char *p2;
  char path[2048];

  memset(path, 0, sizeof(path));
  p = strchr((char*)dir, '/');

  if (!p)
    {
      mkdir(dir, 0755);
      return;
    }
  dst = path;
  p2 = dir;
  while (p)
    {
      int i;

      for ( i = 0; i < (int)(p - p2); i++ )
        {
          *dst = p2[i];
          dst++;
        }
      *dst = '\0';
      p2 = p;

      mkdir(path, 0755);
      p = strchr(p + 1, '/');
    }
  strcat(path, p2);
  mkdir(path, 0755);
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
  create_dir_structure(dir);
  fp = fopen(buf, mode);
  free(buf);
  panic_if(!fp, "Cannot open file %s/%s\n", dir, filename);

  return fp;
}
