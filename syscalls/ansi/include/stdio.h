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

typedef int FILE;

#define EOF (-1)

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

FILE* fopen(const char* path, const char* mode); /* Not generated */
int fclose(FILE* fp); /* Not generated */

void clearerr(FILE* stream); /* Not generated */
int feof(FILE* stream); /* Not generated */
int ferror(FILE* stream); /* Not generated */

/* This is not in ANSI C but a convenient way of finding out how large
 * the file is. It returns DataInputStream.available().
 */
int favail(FILE* stream); /* Not generated */

int fseek(FILE* stream, long offset, int whence); /* Not generated */
int fflush(FILE* stream); /* Not generated */

int fgetc(FILE* stream); /* Not generated */
char* fgets(char* s, int size, FILE* stream); /* Not generated */
int fputc(int c, FILE* stream); /* Not generated */
#define fputs __fputs
int __fputs(const char* ptr, FILE* stream); /* Not generated */

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream); /* Not generated */
size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream); /* Not generated */

extern int vsnprintf(char* str, unsigned int size, const char* format, va_list ap);
#define vsprintf(str, fmt, ap) snprintf(str, 0xfffffff, fmt, ap)
#define vfprintf(str, fmt, ap) fprintf(str, fmt, ap)
extern int vprintf(const char* format, va_list ap);
extern int snprintf(char *buf, size_t n, const char *fmt, ...);
#define sprintf(buf, fmt, x...) snprintf(buf, 0xfffffff, fmt, x)
extern int fprintf(FILE *fp, const char* fmt, ...);
extern int printf(const char* fmt, ...);

void __setup_io(FILE* addr_stdin, FILE* addr_stdout, FILE* addr_stderr); /* Not generated */


/* Does not apply for us yet */
#define setbuf(stream, buf)

#if defined(__cplusplus)
}
#endif
#endif /* !__STDIO_H__ */
