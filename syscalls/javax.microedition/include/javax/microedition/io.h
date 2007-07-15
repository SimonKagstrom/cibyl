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

#include <cibyl.h>
#include <stdio.h> /* FILE* */

#include <java/util.h>

typedef int NOPH_InputStream_t;
typedef int NOPH_OutputStream_t;
typedef int NOPH_DataInputStream_t;
typedef int NOPH_DataOutputStream_t;
typedef int NOPH_Connection_t;
typedef int NOPH_FileConnection_t;
typedef int NOPH_InputConnection_t;
typedef int NOPH_StreamConnection_t;
typedef int NOPH_HttpConnection_t;
typedef int NOPH_RecordStore_t;

/* Network and file stuff */
NOPH_DataInputStream_t NOPH_Connector_openDataInputStream(const char* name); /* Throws */
NOPH_FileConnection_t NOPH_Connector_openFileConnection(const char* name); /* Not generated */
FILE* NOPH_Connector_openFILEInputStream(const char* name); /* Not generated */
FILE* NOPH_Connector_openFILEOutputStream(const char* name); /* Not generated */

NOPH_DataOutputStream_t NOPH_Connector_openDataOutputStream(const char* name); /* Throws */

void NOPH_FileConnection_close(NOPH_FileConnection_t fc); /* Throws */
NOPH_Enumeration_t NOPH_FileConnection_list(NOPH_FileConnection_t fc); /* Throws */

/* Input stream stuff */
int NOPH_InputStream_available(NOPH_InputStream_t is); /* Throws */
int NOPH_InputStream_read(NOPH_InputStream_t is); /* Throws */
int NOPH_InputStream_read_into(NOPH_InputStream_t is, char* vec); /* Throws */
void NOPH_InputStream_close(NOPH_InputStream_t is); /* Throws */

/* Output stream stuff */
void NOPH_OutputStream_write(NOPH_OutputStream_t os, int b); /* Throws */
void NOPH_OutputStream_writeInt(NOPH_OutputStream_t os, int i); /* Throws */
void NOPH_OutputStream_writeShort(NOPH_OutputStream_t os, short i); /* Throws */
void NOPH_OutputStream_close(NOPH_OutputStream_t os); /* Throws */

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

#if defined(__cplusplus)
}
#endif
#endif /* !__JAVAX__MICROEDITION__IO_H__ */
