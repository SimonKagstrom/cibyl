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
  int n_ops;
} all_fops_t;

typedef struct
{
  cibyl_dops_t **table;
  const char   **uris;
  cibyl_dops_t  *fallback;
  int n_ops;
} all_dops_t;

all_fops_t fops;
all_dops_t dops;

static void register_op(all_fops_t *f, const char *uri, cibyl_fops_t *fop, int is_default)
{
  int idx = -1;
  int i;

  for (i = 0; i < f->n_ops; i++)
    {
      if ( strcmp(uri, f->uris[i]) == 0 &&
           fop->priority > f->table[i]->priority)
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
      idx = f->n_ops++;
      f->table = (cibyl_fops_t**)realloc(f->table, sizeof(cibyl_fops_t*) * f->n_ops);
      f->uris = (const char**)realloc(f->uris, sizeof(char*) * f->n_ops);
    }
  NOPH_panic_if( !(f->table && f->uris), "Memory allocation of fops failed");
  f->table[idx] = fop;
  f->uris[idx] = uri;
  if (is_default)
    f->fallback = fop;
}


void cibyl_fops_register(const char *uri, cibyl_fops_t *fop, int is_default)
{
  register_op(&fops, uri, fop, is_default);
}

void cibyl_dops_register(const char *uri, cibyl_dops_t *dop, int is_default)
{
  /* OK. Pretty ugly, but what the heck... */
  register_op((void*)&dops,
              uri, (cibyl_fops_t*)dop, is_default);
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
  {"rb", READ},
  {"rw", READ_WRITE},
  {"r+", READ_WRITE},
  {"w" , WRITE},
  {"wb", WRITE},
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

DIR *cibyl_dir_alloc(cibyl_dops_t *dop)
{
  DIR *out;

  if ( !(out = (DIR*)malloc(sizeof(DIR) + dop->priv_data_size)) )
    return NULL;
  memset(out, 0, sizeof(DIR) + dop->priv_data_size);

  out->ops = dop;
  out->priv = (void*)(out + 1);

  return out;
}

void cibyl_dir_free(DIR *dir)
{
  free(dir);
}

/* ----- */

FILE *fopen(const char *path, const char *in_mode)
{
  cibyl_fops_open_mode_t mode;
  FILE *out = NULL;
  int i;

  mode = cibyl_file_get_mode(in_mode);

  for (i = 0; i < fops.n_ops; i++)
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
  size_t ret;

  if (size == 0)
    return 0;

  /* Do we need to seek? */
  if (fp->fptr != fp->vfptr)
    {
      fp->ops->seek(fp, fp->vfptr - fp->fptr);
      fp->fptr = fp->vfptr;
    }

  /* Read and update the file pointers */
  ret = fp->ops->read(fp, ptr, size);
  fp->fptr += ret;
  fp->vfptr = fp->fptr;

  return ret / in_size;
}

size_t fwrite(const void *ptr, size_t in_size, size_t nmemb, FILE *fp)
{
  size_t size = in_size * nmemb;
  size_t ret;

  if (size == 0)
    return 0;

  ret = fp->ops->write(fp, ptr, size) / in_size;

  fp->fptr += ret;
  fp->vfptr = fp->fptr;
  return ret;
}

int fseek(FILE *fp, long offset, int whence)
{
  long skip = offset;

  switch (whence)
    {
    case SEEK_SET:
      fp->vfptr = 0; /* skip == offset */
      break;
    case SEEK_END:
      fp->vfptr = 0;
      skip = fp->file_size + offset;
      break;
    case SEEK_CUR:
      /* Do nothing */
      break;
    default:
      NOPH_throw(NOPH_Exception_new_string("Invalid seek mode"));
    }

  fp->vfptr += skip;

  return 0;
}

long ftell(FILE *fp)
{
  return fp->vfptr;
}

void rewind(FILE *fp)
{
  fseek(fp, 0L, SEEK_SET);
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
      if (feof(fp))
        return out;
      *s = c;
      s++;
    } while (c != '\0' && c != '\n');

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

DIR *opendir(const char *dirname)
{
  int i;

  for (i = 0; i < dops.n_ops; i++)
    {
      cibyl_dops_t *cur = dops.table[i];
      const char *uri = dops.uris[i];
      int len = strlen(uri);

      if (cur->keep_uri)
        len = 0;

      if (strncmp(uri, dirname, len) == 0)
        return cur->opendir(dirname + len);
    }

  /* Found nothing, return the default */
  if (dops.fallback)
    return dops.fallback->opendir(dirname);

  return NULL;
}

int closedir(DIR *dir)
{
  return dir->ops->closedir(dir);
}

int readdir_r(DIR *dir, struct dirent *entry,
              struct dirent **result)
{
  int out;

  *result = entry;
  if ( (out = dir->ops->readdir(dir, entry)) != 0)
    *result = NULL;
  return out;
}

struct dirent *readdir(DIR *dir)
{
  static struct dirent global_dirent;
  struct dirent *out;

  if ( readdir_r(dir, &global_dirent, &out) != 0 )
    return NULL;

  return &global_dirent;
}
