/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      fileops.c
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   fopen etc impl.
 *
 * $Id:$
 *
 ********************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <cibyl-fileops.h>

typedef struct
{
  cibyl_fops_t **table;
  cibyl_fops_t *fallback;
  int n_fops;
} all_fops_t;

all_fops_t fops;

void cibyl_fops_register(cibyl_fops_t *fop, int is_default)
{
  int idx = fops.n_fops++;

  fops.table = (cibyl_fops_t**)realloc(fops.table, sizeof(cibyl_fops_t*) * fops.n_fops);
  fops.table[idx] = fop;
  if (is_default)
    fops.fallback = fop;
  assert(fops.table);
}

void cibyl_fops_unregister(cibyl_fops_t *fop)
{
  int i;

  for (i = 0; i < fops.n_fops; i++)
    {
      if (fop == fops.table[i])
	{
	  fops.table[i] = fops.table[fops.n_fops - 1];
	  fops.n_fops--;
	  return;
	}
    }
  assert(0 && "fop has not been registered but is unregistered");
}

FILE *cibyl_file_alloc(cibyl_fops_t *fop)
{
  FILE *out;

  if ( !(out = (FILE*)malloc(sizeof(FILE) + fop->priv_data_size)) )
    return NULL;

  out->ops = fop;
  out->priv = (void*)(out + 1);
  return out;
}

void cibyl_file_free(FILE *fp)
{
  free(fp);
}

FILE *fopen(const char *path, const char *in_mode)
{
  FILE *out = NULL;
  cibyl_fops_open_mode_t mode = READ;
  int i;

  if (strchr(in_mode, 'w') != NULL)
    mode = WRITE;
  if (strchr(in_mode, 'a') != NULL)
    mode = APPEND;
  if (strstr(in_mode, "a+") != NULL)
    mode = APPEND;

  for (i = 0; i < fops.n_fops; i++)
    {
      cibyl_fops_t *cur = fops.table[i];
      const char *uri = cur->uri;
      int len = uri ? strlen(uri) : -1;

      if (uri && strncmp(uri, path, len) == 0)
	{
	  /* URI match! */
	  if ( !(out = cibyl_file_alloc(cur)) )
	    return NULL;

	  if (cur->open(out, path + len, mode) < 0)
	    {
	      free(out);
	      return NULL;
	    }
	    return out;
	}
    }

  /* Found nothing, return the default */
  if (fops.fallback)
    {
      if ( !(out = cibyl_file_alloc(fops.fallback)) )
	return NULL;
      if (fops.fallback->open(out, path, mode) < 0)
	{
	  free(out);
	  return NULL;
	}
    }

  return out;
}


int fclose(FILE *fp)
{
  int out = fp->ops->close(fp);

  cibyl_file_free(fp);
  return out;
}


size_t fread(void *ptr, size_t in_size, size_t nmemb, FILE *fp)
{
  size_t size = in_size * nmemb;

  if (size == 0)
    return 0;

  return fp->ops->read(fp, ptr, size) / in_size;
}

size_t fwrite(const void *ptr, size_t in_size, size_t nmemb, FILE *fp)
{
  size_t size = in_size * nmemb;

  if (size == 0)
    return 0;

  return fp->ops->write(fp, ptr, size) / in_size;
}

int fseek(FILE *fp, long offset, int whence)
{
  return fp->ops->seek(fp, offset, whence);
}

long ftell(FILE *fp)
{
  return fp->ops->tell(fp);
}

void rewind(FILE *fp)
{
  fp->ops->seek(fp, 0L, SEEK_SET);
}

void clearerr(FILE* fp)
{
}

int ferror(FILE* fp)
{
  return 0;
}

int fflush(FILE* fp)
{
  if (fp == NULL)
    return 0;
  return fp->ops->flush(fp);
}

int fgetc(FILE* fp)
{
  char out;

  if (fread(&out, 1, 1, fp) < 0)
    {
      /* FIXME: Do some error handling here */
    }

  return out;
}

char* fgets(char* s, int size, FILE* fp)
{
  char *out = s;
  int c;

  do
    {
      c = fgetc(fp);
      *s = c;
      s++;
    } while (c != '\0');

  return out;
}

int fputc(int c, FILE* fp)
{
  if (fwrite(&c, 1, 1, fp) < 0)
    {
      /* FIXME: Do some error handling here */
    }

  return c;
}

int __fputs(const char* ptr, FILE* fp)
{
  char *p;
  int n = 0;

  for (p = (char*)ptr; *p; p++, n++)
    {
      if (fwrite(p, 1, 1, fp) < 0)
        return EOF;
    }

  return n;
}
