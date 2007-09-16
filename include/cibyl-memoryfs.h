/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      cibyl-memoryfs.h
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Memory filesystem stuff
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __CIBYL_MEMORYFS_H__
#define __CIBYL_MEMORYFS_H__

#include <stdio.h>
#include <cibyl-fileops.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct
{
  void *data;
  size_t allocated_size;
  int allocate;
  const char *writeback_path;
  const char *mode;
} NOPH_Memory_file_t;

extern cibyl_fops_t NOPH_Memory_fops;


/**
 * Open a new memory file, potentially allocating memory for it
 *
 * @param ptr a pointer to the file data. If NULL is passed, the data
 *            will be allocated.
 * @param size the size of the data passed (or the size of the
 *            allocated data if @ptr is NULL)
 * @param allocate 1 if this file should be possible to extend
 *
 * @return a pointer to the file object
 */
FILE *NOPH_MemoryFile_open(void *ptr, size_t size, int allocate);

/**
 * Setup a new memory file, potentially allocating memory for it
 *
 * @param fp the FILE pointer to setup
 * @param ptr a pointer to the file data. If NULL is passed, the data
 *            will be allocated.
 * @param size the size of the data passed (or the size of the
 *            allocated data if @ptr is NULL)
 * @param allocate 1 if this file should be possible to extend
 *
 * @throws NOPH_OutOfMemoryException_t if the memory runs out
 */
void NOPH_MemoryFile_setup(FILE *fp, void *ptr, size_t size, int allocate);

/**
 * Open a memory file indirectly. This will open a regular file with
 * the standard fopen, read in all of it into memory and create a
 * memory file from the allocated memory.
 *
 * @param name the name of the file to open (passed to fopen)
 * @param mode the open mode (pass to fopen)
 *
 * @return a pointer the opened memory file or NULL if the open failed
 */
FILE *NOPH_MemoryFile_openIndirect(const char *name, const char *mode);

/**
 * Return the pointer to the beginning of the data in the file
 *
 * @param fp the file to return the data for
 *
 * @return a pointer to the start of data
 */
void *NOPH_MemoryFile_getDataPtr(FILE *fp);

#if defined(__cplusplus)
}
#endif

#endif /* !__CIBYL_MEMORYFS_H__ */
