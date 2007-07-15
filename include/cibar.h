/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      cibar.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Cibyl "ar"-like structure
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __CIBAR_H__
#define __CIBAR_H__

#include <stdio.h>
#include <stdint.h>

typedef struct
{
  uint8_t   *data;
  size_t     data_size;
  struct s_cibar *cibar;

  char      *name;
} cibar_file_entry_t;

typedef struct s_cibar
{
  int           n_files;
  uint8_t      *data;
  char         *strings;
  int           refcount;

  cibar_file_entry_t files[];
} cibar_t;

typedef struct
{
  cibar_file_entry_t *entry;
  int fp;
} cibar_file_t;

cibar_t *cibar_open(FILE *f);
void cibar_close(cibar_t *p);

/* Works like fseek / fread / fclose / fopen (but these are read-only) */
int cibar_file_seek(cibar_file_t *p, long offset, int whence);
int cibar_file_tell(cibar_file_t *p);
size_t cibar_file_read(cibar_file_t *p, void *ptr, size_t size, size_t nmemb);
void cibar_file_close(cibar_file_t *p);
cibar_file_t *cibar_file_open(cibar_t *p, const char *name);
static inline int cibar_file_eof(cibar_file_t *p)
{
  return (p->fp >= p->entry->data_size);
}

#endif /* !__CIBAR_H__ */
