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
FILE *NOPH_memoryFile_open(void *ptr, size_t size, int allocate);

/**
 * Return the pointer to the beginning of the data in the file
 *
 * @param fp the file to return the data for
 *
 * @return a pointer to the start of data
 */
void *NOPH_memoryFile_getDataPtr(FILE *fp);

#endif /* !__CIBYL_MEMORYFS_H__ */
