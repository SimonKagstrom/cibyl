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
#include <dirent.h>

#if defined(__cplusplus)
extern "C" {
#endif

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
  READ_TRUNCATE  /* "w+" */
} cibyl_fops_open_mode_t;

/**
 * File operations structure
 */
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

  int (*flush)(FILE *fp); /* flush the stream */
  int (*remove)(const char *pathname); /* Remove a file (might not be supported) */
} cibyl_fops_t;

/**
 * Directory operations structure
 */
typedef struct s_cibyl_dops
{
  const size_t priv_data_size;
  int keep_uri;
  int priority;

  DIR *(*opendir)(const char *dirname);
  int (*readdir)(DIR *dir, struct dirent *entry);
  int (*closedir)(DIR *dir);
} cibyl_dops_t;

/**
 * Register a new "filesystem" with Cibyl.
 *
 * @param uri the URI to register the filesystem with. Must not be free'd
 * @param fop the file operations structure to register
 * @param is_default boolean value to tell if this should be the
 *        default filesystem (i.e., fallback if no URI match)
 */
void cibyl_fops_register(const char *uri, cibyl_fops_t *fop, int is_default);

/**
 * Register a new "directory handler" with Cibyl.
 *
 * @param uri the URI to register the filesystem with. Must not be free'd
 * @param dop the file operations structure to register
 * @param is_default boolean value to tell if this should be the
 *        default filesystem (i.e., fallback if no URI match)
 */
void cibyl_dops_register(const char *uri, cibyl_dops_t *dop, int is_default);

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
 * Free a DIR structure
 *
 * @param dop the DIR ops
 *
 * @return a pointer to the dir
 */
DIR *cibyl_dir_alloc(cibyl_dops_t *dop);

/**
 * Allocate a FILE structure
 *
 * @param fp the FILE structure to free
 */
void cibyl_file_free(FILE *fp);

/**
 * Free a DIR structure
 *
 * @param dir the DIR to free
 */
void cibyl_dir_free(DIR *dir);

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

#if defined(__cplusplus)
}
#endif

#endif /* !__CIBYL_FILEOPS_H__ */
