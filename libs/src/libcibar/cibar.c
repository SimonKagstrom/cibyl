/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      cibyl-fileops.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   File operations
 *
 * $Id:$
 *
 ********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <cibar.h>

#define min(x,y) ( (x) < (y) ? (x) : (y) )
#define max(x,y) ( (x) > (y) ? (x) : (y) )

static void increase_refcount(cibar_t *p)
{
  p->refcount++;
}

static void decrease_refcount(cibar_t *p)
{
  p->refcount--;

  if (p->refcount <= 0)
    {
      free(p->strings);
      free(p->data);
      free(p);
    }
}

int cibar_file_seek(cibar_file_t *p, long offset, int whence)
{
  int old = p->fp;

  if (whence == SEEK_SET)
    p->fp = offset;
  else if (whence == SEEK_CUR)
    p->fp = p->fp + offset;
  else if (whence == SEEK_END)
    p->fp = p->entry->data_size + offset;
  else
    return -1;

  if (p->fp < 0)
    {
      p->fp = old;
      return -1;
    }

  if (p->fp >= p->entry->data_size)
    p->fp = p->entry->data_size;

  return 0;
}

int cibar_file_tell(cibar_file_t *p)
{
  return p->fp;
}

size_t cibar_file_read(cibar_file_t *p, void *ptr, size_t size, size_t nmemb)
{
  size_t cnt = min(size * nmemb, p->entry->data_size - p->fp);

  if (cnt >= 0)
    memcpy( ptr, p->entry->data + p->fp, cnt );
  p->fp = p->fp + cnt;

  return cnt / size;
}

cibar_file_t *cibar_file_open(cibar_t *p, const char *name)
{
  int i;

  for (i = 0; i < p->n_files; i++)
    {
      if (strlen(p->files[i].name) == strlen(name) &&
          strcmp(p->files[i].name, name) == 0)
        {
          cibar_file_t *out = malloc(sizeof(cibar_file_t));

          increase_refcount(p);
          out->entry = &p->files[i];
          out->fp = 0;

          return out;
        }
    }

  return NULL;
}

void cibar_file_close(cibar_file_t *p)
{
  decrease_refcount(p->entry->cibar);
  free(p);
}


typedef struct
{
  uint32_t strtab_offset;
  uint32_t data_offset;
  uint32_t data_size;
} cibar_internal_file_entry_t;

typedef struct
{
  uint32_t magic;
  uint32_t total_length;
  uint32_t strtab_length;
  uint32_t data_length;
  uint32_t n_file_entries;
  uint32_t data_start;

  cibar_file_entry_t entries[];
} cibar_internal_t;

void cibar_close(cibar_t *p)
{
  decrease_refcount(p);
}

cibar_t *cibar_open(FILE *f)
{
  cibar_internal_file_entry_t *entries;
  char *strings;
  uint8_t *data;
  cibar_internal_t cb;
  cibar_t *out;
  int i;

  /* Read the header */
  fread(&cb, sizeof(cibar_internal_t), 1, f);

  if (cb.magic != 0x12344321)
    {
      printf("cibar: Wrong magic: 0x%08x\n", cb.magic);
      goto error_1;
    }

  /* Allocate structures */
  if ( !(entries = (cibar_internal_file_entry_t*)malloc(sizeof(cibar_internal_file_entry_t) * cb.n_file_entries)) )
    goto error_1;
  if ( !(strings = (char*)malloc( cb.strtab_length )) )
    goto error_2;
  if ( !(data = (uint8_t*)malloc( cb.data_length )) )
    goto error_3;
  if ( !(out = (cibar_t*)malloc( sizeof(cibar_t) + sizeof(cibar_file_entry_t) * cb.n_file_entries)) )
    goto error_4;

  /* Read the rest of the file */
  if (fread(entries, 1, sizeof(cibar_internal_file_entry_t) * cb.n_file_entries, f) !=
      sizeof(cibar_internal_file_entry_t) * cb.n_file_entries)
    goto error_4;
  if (fread(strings, 1, cb.strtab_length, f) != cb.strtab_length)
    goto error_4;
  if (fread(data, 1, cb.data_length, f) != cb.data_length)
    goto error_4;

  /* Fill in the out stucture */
  out->refcount = 1;
  out->n_files = cb.n_file_entries;
  out->strings = strings;
  out->data = data;
  for (i = 0; i < cb.n_file_entries; i++)
    {
      cibar_file_entry_t *p = &out->files[i];

      p->cibar = out;
      p->data = data + entries[i].data_offset;
      p->data_size = entries[i].data_size;
      p->name = strings + entries[i].strtab_offset;
    }

  free(entries);

  return out;
 error_4:
  free(data);
 error_3:
  free(strings);
 error_2:
  free(entries);
 error_1:
  return NULL;
}
