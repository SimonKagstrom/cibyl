/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      utils.h
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Some utility functions
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __UTILS_H__
#define __UTILS_H__

#include <arpa/inet.h> /* htonl */
#include <stdlib.h>

void *xcalloc(size_t nmemb, size_t size);

void *xrealloc(void *ptr, size_t size);

static inline uint32_t be32_to_host(uint32_t in)
{
  return ntohl(in);
}

static inline uint32_t signext_16(uint16_t in)
{
  int out = (uint32_t)in;

  if (out & (1<<15))
    return out | 0xffff0000;

  return out;
}

typedef struct
{
  void *data;
  size_t size;
} file_chunk_t;

file_chunk_t *read_file(const char *filename);

#endif /* !__UTILS_H__ */
