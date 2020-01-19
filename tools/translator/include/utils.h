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
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//#define max(x,y) ( (x) > (y) ? (x) : (y) )

void *xcalloc(size_t nmemb, size_t size);

void *xrealloc(void *ptr, size_t size);

/* Byte swap with the native size */
unsigned long be_to_host(unsigned long in);

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

int file_exists(const char *fmt, ...);

void *read_file(size_t *out_size, const char *fmt, ...);

void *read_cpp(size_t *out_size, const char **defines, const char *fmt, ...);


DIR *open_dir_fmt(const char *fmt, ...);

FILE *open_file_in_dir(const char *dir, const char *filename, const char *mode);

#define panic(x...) do \
  { \
    fprintf(stderr, "============Translator panic===========\n"); \
    fprintf(stderr, x); \
    fprintf(stderr, "=======================================\n"); \
    exit(1); \
  } while(0)

#define panic_if(cond, x...) \
  do { if ((cond)) panic(x); } while(0)

static inline char *xstrdup(const char *s)
{
  char *out = strdup(s);

  panic_if(!out, "strdup failed");

  return out;
}

#define xsnprintf(buf, size, fmt, x...) do { \
    int r = snprintf(buf, size, fmt, x); \
    panic_if(r < 0 || r >= (int)(size), "snprintf failed for %s with %d\n", fmt, r); \
} while(0)

#endif /* !__UTILS_H__ */
