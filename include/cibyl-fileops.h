/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      cibyl-fileops.h
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Fileops definitions
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __CIBYL_FILEOPS_H__
#define __CIBYL_FILEOPS_H__
#include <stdio.h>

typedef enum
{
  READ,          /* "r"  */
  READ_WRITE,    /* "r+"  */
  WRITE,         /* "w"  */
  APPEND,        /* "a"  */
  READ_APPEND,   /* "a+"  */
  READ_TRUNCATE, /* "w+" */
} cibyl_fops_open_mode_t;

typedef struct s_cibyl_fops
{
  const char *uri;  /* The uri used to identify this mode */
  const size_t priv_data_size;
  int keep_uri;

  /* Open a file, return -1 if failed */
  FILE *(*open)(const char *path, cibyl_fops_open_mode_t mode);
  int (*close)(FILE *fp);  /* Close the file  */

  size_t (*read)(FILE *fp, void *dst, size_t amount);  /* Read from the file  */
  size_t (*write)(FILE *fp, const void *src, size_t amount);  /* Write to the file  */

  int (*seek)(FILE *fp, long offset, int whence);  /* Move the file pointer  */
  long (*tell)(FILE *fp);  /* Return the position of the file pointer (ftell)  */
  int (*eof)(FILE *fp);  /* is this the end-of-file?  */

  int (*flush)(FILE *fp); /* flush the stream */
} cibyl_fops_t;

/**
 * Register a new "filesystem" with Cibyl.
 *
 * @param fops the file operations structure to register
 * @param is_default boolean value to tell if this should be the
 *        default filesystem (i.e., fallback if no URI match)
 */
void cibyl_register_fops(cibyl_fops_t *fops, int is_default);

/**
 * Deregister a "filesystem"
 *
 * @param fops the filesystem to deregister
 */
void cibyl_unregister_fops(cibyl_fops_t *fops);

/**
 * Allocate a new FILE structure and set it up for @a fop
 *
 * @param fop the fop to associate with the file
 *
 * @throws NOPH_OutOfMemoryException if memory is up
 * @return a pointer to the new file
 */
FILE *cibyl_file_alloc(cibyl_fops_t *fop);

/**
 * Allocate a FILE structure
 *
 * @param fp the FILE structure to free
 *
 * @throws NOPH_OutOfMemoryException if memory is out
 */
void cibyl_file_free(FILE *fp);

#endif /* !__CIBYL_FILEOPS_H__ */
