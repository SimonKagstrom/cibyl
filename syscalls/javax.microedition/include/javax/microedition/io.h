/*********************************************************************
 *
 * Copyright (C) 2006-2007,  Blekinge Institute of Technology
 *
 * Filename:      io.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:
 *
 * $Id: io.h 13699 2007-02-18 20:26:15Z ska $
 *
 ********************************************************************/
#ifndef __JAVAX__MICROEDITION__IO_H__
#define __JAVAX__MICROEDITION__IO_H__
#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @file javax/microedition/io.h Functions from the
 * javax.microedition.io package (plus some helpers)
 */

#include <cibyl.h>
#include <stdio.h> /* FILE* */

#include <java/util.h>
#include <java/io.h>

#define NOPH_Connector_READ       1
#define NOPH_Connector_WRITE      2
#define NOPH_Connector_READ_WRITE 3

typedef int NOPH_Connection_t;
typedef int NOPH_FileConnection_t;
typedef int NOPH_InputConnection_t;
typedef int NOPH_StreamConnection_t;
typedef int NOPH_HttpConnection_t;
typedef int NOPH_RecordStore_t;

/* Connector */
NOPH_DataInputStream_t NOPH_Connector_openDataInputStream(const char* name); /* Throws */
NOPH_FileConnection_t NOPH_Connector_openFileConnection_mode(const char* name, int mode); /* Not generated */

static inline NOPH_FileConnection_t NOPH_Connector_openFileConnection(const char* name)
{
  return NOPH_Connector_openFileConnection_mode(name, NOPH_Connector_READ);
}

/* FileConnection (JSR-075) */
NOPH_InputStream_t NOPH_FileConnection_openInputStream(NOPH_FileConnection_t fc); /* Throws */
NOPH_OutputStream_t NOPH_FileConnection_openOutputStream(NOPH_FileConnection_t fc, long offset); /* Throws */
NOPH_DataInputStream_t NOPH_FileConnection_openDataInputStream(NOPH_FileConnection_t fc); /* Throws */
NOPH_DataOutputStream_t NOPH_FileConnection_openDataOutputStream(NOPH_FileConnection_t fc); /* Throws */
bool_t NOPH_FileConnection_exists(NOPH_FileConnection_t fc); /* Throws */
bool_t NOPH_FileConnection_canWrite(NOPH_FileConnection_t fc); /* Throws */
bool_t NOPH_FileConnection_canRead(NOPH_FileConnection_t fc); /* Throws */
bool_t NOPH_FileConnection_isOpen(NOPH_FileConnection_t fc); /* Throws */
void NOPH_FileConnection_truncate(NOPH_FileConnection_t fc, long byteOffest); /* Throws */
int NOPH_FileConnection_fileSize(NOPH_FileConnection_t fc); /* Throws */
void NOPH_FileConnection_create(NOPH_FileConnection_t fc); /* Throws */
void NOPH_FileConnection_delete(NOPH_FileConnection_t fc); /* Throws */
void NOPH_FileConnection_close(NOPH_FileConnection_t fc); /* Throws */
NOPH_Enumeration_t NOPH_FileConnection_list(NOPH_FileConnection_t fc); /* Throws */


NOPH_DataOutputStream_t NOPH_Connector_openDataOutputStream(const char* name); /* Throws */

/* Record-store stuff */
NOPH_RecordStore_t NOPH_RecordStore_openRecordStore(char* recordStoreName, bool_t createIfNecessary); /* Throws */
void NOPH_RecordStore_closeRecordStore(NOPH_RecordStore_t rs); /* Throws */
void NOPH_RecordStore_deleteRecordStore(char* recordStoreName); /* Throws */
void NOPH_RecordStore_deleteRecord(NOPH_RecordStore_t rs, int recordId); /* Throws */
int NOPH_RecordStore_getNumRecords(NOPH_RecordStore_t rs); /* Throws */
int NOPH_RecordStore_getRecordSize(NOPH_RecordStore_t rs, int recordId); /* Throws */

int NOPH_RecordStore_addRecord(NOPH_RecordStore_t rs, char* newData, int offset, int numBytes); /* Not generated */
void NOPH_RecordStore_setRecord(NOPH_RecordStore_t rs, int recordId, char* newData, int offset, int numBytes); /* Not generated */
int NOPH_RecordStore_getRecord(NOPH_RecordStore_t rs, int recordId, char* buffer, int offset); /* Not generated */

/* -- Non-java functionality -- */
/**
 * Create a ANSI C file from a FileConnection. This is a convenience
 * method to use instead of creating a file connection, opening a
 * stream from it and convert it to a FILE ptr.
 *
 * @param name the name of the file to open
 * @param mode the mode to open the file in
 *
 * @return a pointer to the new FILE input stream
 */
extern FILE *NOPH_FileConnection_openFILE(const char *name, const char *mode);


#if defined(__cplusplus)
}
#endif
#endif /* !__JAVAX__MICROEDITION__IO_H__ */
