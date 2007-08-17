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

#include <java/lang.h>

typedef struct
{
  cibyl_fops_t **table;
  const char   **uris;
  cibyl_fops_t  *fallback;
  int n_fops;
} all_fops_t;

all_fops_t fops;

void cibyl_fops_register(const char *uri, cibyl_fops_t *fop, int is_default)
{
  int idx = -1;
  int i;

  for (i = 0; i < fops.n_fops; i++)
    {
      if ( strcmp(uri, fops.uris[i]) == 0 &&
           fop->priority > fops.table[i]->priority)
        {
          /* Replace the current one if the uris match and the
           *  priority is above the old one */
          idx = i;
          break;
        }
    }

  if (idx == -1)
    {
      /* Not found, make space for it */
      idx = fops.n_fops++;
      fops.table = (cibyl_fops_t**)realloc(fops.table, sizeof(cibyl_fops_t*) * fops.n_fops);
      fops.uris = (const char**)realloc(fops.uris, sizeof(char*) * fops.n_fops);
    }
  NOPH_panic_if( !(fops.table && fops.uris), "Memory allocation of fops failed");
  fops.table[idx] = fop;
  fops.uris[idx] = uri;
  if (is_default)
    fops.fallback = fop;
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
	}
    }
}

FILE *cibyl_file_alloc(cibyl_fops_t *fop)
{
  FILE *out;

  if ( !(out = (FILE*)malloc(sizeof(FILE) + fop->priv_data_size)) )
    return NULL;

  /* Zero everything by default */
  memset(out, 0, sizeof(FILE) + fop->priv_data_size);

  out->ops = fop;
  out->priv = (void*)(out + 1);

  return out;
}

void cibyl_file_free(FILE *fp)
{
  free(fp);
}

typedef struct
{
  const char *str;
  cibyl_fops_open_mode_t mode;
} mode_t;

static mode_t mode_mapping[] =
{
  {"r" , READ},
  {"r+", READ_WRITE},
  {"w" , WRITE},
  {"a" , APPEND},
  {"a+", READ_APPEND},
  {"w+", READ_TRUNCATE},
  {NULL, READ}
};

cibyl_fops_open_mode_t cibyl_file_get_mode(const char *in_mode)
{
  mode_t *m = NULL;
  int i;

  /* Lookup the mode among the mode strings */
  for (i = 0; i < sizeof(mode_mapping) / sizeof(mode_t); i++)
    {
      if (strcmp(in_mode, mode_mapping[i].str) == 0)
        m = &mode_mapping[i];
    }
  if (!m)
    NOPH_throw(NOPH_Exception_new_string("Unsupported mode for fopen"));

  return m->mode;
}

FILE *fopen(const char *path, const char *in_mode)
{
  cibyl_fops_open_mode_t mode;
  FILE *out = NULL;
  int i;

  mode = cibyl_file_get_mode(in_mode);

  for (i = 0; i < fops.n_fops; i++)
    {
      cibyl_fops_t *cur = fops.table[i];
      const char *uri = fops.uris[i];
      int len = uri ? strlen(uri) : -1;

      if (uri && strncmp(uri, path, len) == 0)
	{
	  /* URI match! */
          if (cur->keep_uri)
            len = 0;
	  if ( !(out = cur->open(path + len, mode)) )
            return NULL;
          return out;
	}
    }

  /* Found nothing, return the default */
  if (fops.fallback)
    {
      if ( !(out = fops.fallback->open(path, mode)) )
        return NULL;
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
  fp->error = 0;
  fp->eof = 0;
}

int ferror(FILE* fp)
{
  return fp->error;
}

int feof(FILE *fp)
{
  return fp->eof;
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
