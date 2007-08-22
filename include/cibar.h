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

  cibar_file_entry_t files[];
} cibar_t;

typedef struct
{
  cibar_file_entry_t *entry;
  int fp;
} cibar_file_t;

/**
 * Open a cibar
 *
 * @param f the file to read the cibar from
  *
 * @return a pointer to the new cibar
 */
cibar_t *cibar_open(FILE *f);

/**
 * Close the cibar. All files opened from the cibar should be closed
 * first since all data is freed
 */
void cibar_close(cibar_t *p);

/**
 * Open a file in the cibar
 *
 * @param p the cibar to open from
 * @param name the name of the file
 *
 * @return a FILE pointer to the cibar
 */
FILE *cibar_file_open(cibar_t *p, const char *name);;

#endif /* !__CIBAR_H__ */
