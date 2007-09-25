/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      stdio.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Cibyl stdio.h
 *
 * $Id: stdio.h 13559 2007-02-09 22:36:31Z ska $
 *
 ********************************************************************/
#ifndef __STDIO_H__
#define __STDIO_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <cibyl.h>
#include <stddef.h>
#include <stdarg.h>

int __puts(const char* string); /* Not generated */
#define puts __puts
int __putchar(int c); /* Not generated */
#define putchar __putchar

/* File handling */
#define SEEK_SET        0       /* Seek from beginning of file.  */
#define SEEK_CUR        1       /* Seek from current position.  */
#define SEEK_END        2       /* Seek from end of file.  */

#ifndef EOF
# define EOF (-1)
#endif

struct s_cibyl_fops;

typedef struct
{
  struct s_cibyl_fops *ops;
  short   eof;   /* eof status */
  short   error;
  long    fptr;  /* Real file pointer */
  long    vfptr; /* Virtual file pointer */
  size_t  file_size;
  void   *priv;  /* Filesystem private stuff */
} FILE;

#define EOF (-1)

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

extern FILE* fopen(const char* path, const char* mode); /* Not generated */
extern int fclose(FILE* fp); /* Not generated */

extern void clearerr(FILE* stream); /* Not generated */
extern int feof(FILE* stream); /* Not generated */
extern int ferror(FILE* stream); /* Not generated */


extern void rewind(FILE *stream);
extern int fseek(FILE* stream, long offset, int whence); /* Not generated */
extern long ftell(FILE *stream);
extern int fflush(FILE* stream); /* Not generated */

extern int fgetc(FILE* stream); /* Not generated */
extern char* fgets(char* s, int size, FILE* stream); /* Not generated */
extern int fputc(int c, FILE* stream); /* Not generated */
#define fputs __fputs
extern int __fputs(const char* ptr, FILE* stream); /* Not generated */

extern size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream); /* Not generated */
extern size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream); /* Not generated */

extern int vsnprintf(char* str, unsigned int size, const char* format, va_list ap);
#define vsprintf(str, fmt, ap) vsnprintf(str, 0xfffffff, fmt, ap)
extern int vfprintf(FILE *fp, const char* fmt, va_list ap);
extern int vprintf(const char* format, va_list ap);
extern int snprintf(char *buf, size_t n, const char *fmt, ...);
#define sprintf(buf, fmt, x...) snprintf(buf, 0xfffffff, fmt, x)
extern int fprintf(FILE *fp, const char* fmt, ...);
extern int printf(const char* fmt, ...);

void __setup_io(void* addr_stdout, void* addr_stderr); /* Not generated */

/* Does not apply for us yet */
#define setbuf(stream, buf)

#if defined(__cplusplus)
}
#endif
#endif /* !__STDIO_H__ */
