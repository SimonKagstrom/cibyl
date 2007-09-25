/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      printf.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Printf implementation by Espen Skoglund
 *
 * $Id: printf.c 13453 2007-02-05 16:28:37Z ska $
 *
 ********************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

int snprintf(char *buf, size_t n, const char *fmt, ...)
{
    va_list ap;
    int r;

    /*
     * Safety check
     */
    if ( fmt == NULL )
	return 0;

    /*
     * Print into buffer.
     */
    va_start(ap, fmt);
    r = vsnprintf(buf, n, fmt, ap);
    va_end(ap);

    return r;
}

/*
 * Function printf (fmt, ...)
 *
 *    Print formated string to terminal like printf(3).
 *
 */
int fprintf(FILE *fp, const char *fmt, ...)
{
    char outbuf[256];
    va_list ap;
    int r;

    /*
     * Safety check
     */
    if ( fmt == NULL )
	return 0;

    /*
     * Print into buffer.
     */
    va_start(ap, fmt);
    r = vsnprintf(outbuf, sizeof(outbuf), fmt, ap);
    va_end(ap);

    /*
     * Output to terminal.
     */
    if ( r > 0 )
	fputs(outbuf, fp);

    return r;
}

int printf(const char *fmt, ...)
{
    char outbuf[256];
    va_list ap;
    int r;

    /*
     * Safety check
     */
    if ( fmt == NULL )
	return 0;

    /*
     * Print into buffer.
     */
    va_start(ap, fmt);
    r = vsnprintf(outbuf, sizeof(outbuf), fmt, ap);
    va_end(ap);

    /*
     * Output to terminal.
     */
    if ( r > 0 )
	fputs(outbuf, stdout);

    fflush(stdout);

    return r;
}

int vprintf(const char *fmt, va_list ap)
{
    char outbuf[256];
    int r;

    /*
     * Safety check
     */
    if ( fmt == NULL )
	return 0;

    /*
     * Print into buffer.
     */
    r = vsnprintf(outbuf, sizeof(outbuf), fmt, ap);

    /*
     * Output to terminal.
     */
    if ( r > 0 )
	fputs(outbuf, stdout);

    fflush(stdout);
    return r;
}

int vfprintf(FILE *fp, const char *fmt, va_list ap)
{
    char outbuf[256];
    int r;

    /*
     * Safety check
     */
    if ( fmt == NULL )
	return 0;

    /*
     * Print into buffer.
     */
    r = vsnprintf(outbuf, sizeof(outbuf), fmt, ap);

    /*
     * Output to terminal.
     */
    if ( r > 0 )
	fputs(outbuf, fp);

    return r;
}
