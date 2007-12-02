/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      io.h
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __JAVA__IO_H__
#define __JAVA__IO_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <cibyl.h>
#include <cibyl-fileops.h>
#include <stdio.h>

typedef int NOPH_InputStream_t;
typedef int NOPH_OutputStream_t;
typedef int NOPH_DataInputStream_t;
typedef int NOPH_DataOutputStream_t;

typedef int NOPH_EOFException_t;

/* Input stream stuff */
int NOPH_InputStream_available(NOPH_InputStream_t is); /* Throws */
int NOPH_InputStream_read(NOPH_InputStream_t is); /* Throws */
int NOPH_InputStream_read_into(NOPH_InputStream_t is, char* vec, int size, short* eof_ptr); /* Not generated */
void NOPH_InputStream_reset(NOPH_InputStream_t is); /* Throws */
void NOPH_InputStream_mark(NOPH_InputStream_t is, int readLimit);
void NOPH_InputStream_markSupported(NOPH_InputStream_t is); /* Throws */
void NOPH_InputStream_close(NOPH_InputStream_t is); /* Throws */
int NOPH_InputStream_skip(NOPH_InputStream_t is, int n); /* Throws */

/* Output stream stuff */
void NOPH_OutputStream_write(NOPH_OutputStream_t os, int b); /* Throws */
void NOPH_OutputStream_flush(NOPH_OutputStream_t os); /* Throws */
void NOPH_OutputStream_close(NOPH_OutputStream_t os); /* Throws */

void NOPH_DataOutputStream_writeInt(NOPH_DataOutputStream_t os, int i); /* Throws */
void NOPH_DataOutputStream_writeShort(NOPH_DataOutputStream_t os, short i); /* Throws */

/* EOFException */
NOPH_EOFException_t NOPH_EOFException_new(void);
NOPH_EOFException_t NOPH_EOFException_new_string(char* s);

/* --- Non-Java functionality --- */
/* For "inheriting" input and output file systems */
extern cibyl_fops_t NOPH_InputStream_fops;
extern cibyl_fops_t NOPH_OutputStream_fops;

typedef struct
{
  NOPH_InputStream_t is;
} NOPH_InputStream_file_t;

typedef struct
{
  NOPH_OutputStream_t os;
} NOPH_OutputStream_file_t;

/**
 * Create a ANSI C file from an OutputStream. The resulting FILE is
 * write-only and does not support fseek, feof etc.
 *
 * @param os the output stream to use as base
 * @return a pointer to the new FILE output stream
 */
extern FILE *NOPH_OutputStream_createFILE(NOPH_OutputStream_t os);

/**
 * Create a ANSI C file from an InputStream. The resulting FILE is
 * read-only and supports normal operations such as fseek, feof etc.
 *
 * @param os the input stream to use as base
 * @return a pointer to the new FILE input stream
 */
extern FILE *NOPH_InputStream_createFILE(NOPH_InputStream_t os);

#if defined(__cplusplus)
}
#endif

#endif /* !__JAVA__IO_H__ */
