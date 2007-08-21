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

/**
 * @file cibyl-fileops.h Describes Cibyl "filesystems" (implementations
 * of the ANSI C file operations)
 */

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
  const size_t priv_data_size;
  int keep_uri;
  int priority;

  /* Open a file, return -1 if failed */
  FILE *(*open)(const char *path, cibyl_fops_open_mode_t mode);
  int (*close)(FILE *fp);  /* Close the file  */

  size_t (*read)(FILE *fp, void *dst, size_t amount);  /* Read from the file  */
  size_t (*write)(FILE *fp, const void *src, size_t amount);  /* Write to the file  */

  void (*seek)(FILE *fp, long offset);  /* Move the file pointer relative to the current position  */
  long (*tell)(FILE *fp);  /* Return the position of the file pointer (ftell)  */

  int (*flush)(FILE *fp); /* flush the stream */
} cibyl_fops_t;

/**
 * Register a new "filesystem" with Cibyl.
 *
 * @param uri the URI to register the filesystem with. Must not be free'd
 * @param fops the file operations structure to register
 * @param is_default boolean value to tell if this should be the
 *        default filesystem (i.e., fallback if no URI match)
 */
void cibyl_fops_register(const char *uri, cibyl_fops_t *fop, int is_default);

/**
 * Deregister a "filesystem"
 *
 * @param fops the filesystem to deregister
 */
void cibyl_fops_unregister(cibyl_fops_t *fops);

/**
 * Allocate a new FILE structure and set it up for @a fop. The entire
 * structure will be zeroed at allocation-time.
 *
 * @param fop the fop to associate with the file
 *
 * @throws NOPH_OutOfMemoryException_t if memory is up
 *
 * @return a pointer to the new file
 */
FILE *cibyl_file_alloc(cibyl_fops_t *fop);

/**
 * Allocate a FILE structure
 *
 * @param fp the FILE structure to free
 */
void cibyl_file_free(FILE *fp);

/**
 * Return the @a cibyl_fops_open_mode_t mode for a given fopen-style
 * mode
 *
 * @param mode the mode to convert
 *
 * @return the converted mode
 *
 * @throws NOPH_Exception_t if the mode cannot be converted
 */
cibyl_fops_open_mode_t cibyl_file_get_mode(const char *mode);

#endif /* !__CIBYL_FILEOPS_H__ */
