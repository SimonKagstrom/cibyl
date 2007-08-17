/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      recordstore-filesystem.c
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Record store fs
 *
 * $Id:$
 *
 ********************************************************************/
#include <cibyl-fileops.h>
#include <cibyl-memoryfs.h>
#include <javax/microedition/io.h>
#include <stdio.h>
#include <stdlib.h>

static cibyl_fops_t record_store_fops;

typedef struct
{
  NOPH_Memory_file_t memfs;
  NOPH_RecordStore_t rs;
  int id;
  void (*close_helper)(FILE *fp);
} record_store_file_t;

static void close_write(FILE *fp)
{
  record_store_file_t *p = (record_store_file_t *)fp->priv;
  void *data = NOPH_MemoryFile_getDataPtr(fp);
  long size = ftell(fp);

  /* Update this record - this can very well throw an exception */
  NOPH_RecordStore_setRecord(p->rs, p->id, data, 0, size);
}

static void close_read(FILE *fp)
{
  /* Do nothing */
}

static void *get_record(NOPH_RecordStore_t rs, int id)
{
  void *data;
  int error;

  NOPH_try(NOPH_setter_exception_handler, (void*)&error)
    {
      int rs_size = NOPH_RecordStore_getRecordSize(rs, id);

      data = malloc(rs_size);
      if (!data)
        return NULL;
      /* Read the entire record into data */
      NOPH_RecordStore_getRecord(rs, id, data, 0);
    } NOPH_catch();
  if (error)
    return NULL;
  return data;
}

/* Format for record stores:
 *    recordstore://store-name:record-number
 *
 * (the recordstore:// part is removed by fopen)
 */
static FILE *open(const char *path,
                  cibyl_fops_open_mode_t mode)
{
  FILE *fp;
  record_store_file_t *p;
  char *record_number = strchr(path, ':');
  char *store_name;
  void *data = NULL;
  int rs_size = 8192; /* assume mode == TRUNCATE */
  int error;

  if (!record_number)
    return NULL;

  store_name = strdup(path);
  *strchr(store_name, ':') = '\0'; /* MUST exist because of above */

  if ( !(fp = cibyl_file_alloc(&record_store_fops)) )
    return NULL;
  p = (record_store_file_t *)fp->priv;

  /* Open the store and set the id */
  NOPH_try(NOPH_setter_exception_handler, (void*)&error)
    {
      p->rs = NOPH_RecordStore_openRecordStore(store_name,
                                               mode == READ ? 0 : 1);
    } NOPH_catch();
  if (error)
    {
      free(store_name);
      cibyl_file_free(fp);
      return NULL;
    }
  p->id = atoi(record_number + 1);
  p->close_helper = close_write; /* assume write */
  free(store_name);

  switch (mode)
    {
    case READ_TRUNCATE:
      /* Just allocate space without reading anything */
      break;
    case WRITE:
    case READ_APPEND:
      /* Write or append - read into a buffer */
      if ( !(data = get_record(p->rs, p->id)) )
        goto err;
    case READ:
      /* Open and read everything into a buffer */
      if ( !(data = get_record(p->rs, p->id)) )
        goto err;
      /* Do not update on close */
      p->close_helper = close_read;
      break;
    default:
      NOPH_panic("Invalid mode %d", mode);
      return NULL;
    }

  /* Create the memory file */
  NOPH_MemoryFile_setup(fp, data, rs_size, 1);

  if (mode == APPEND)
    fseek(fp, 0, SEEK_END);

  return fp;
 err:
  free(fp);
  return NULL;
}

static int close(FILE *fp)
{
  record_store_file_t *p = (record_store_file_t *)fp->priv;

  /* Call the helper to (sometimes) save the record data */
  p->close_helper(fp);
  NOPH_RecordStore_closeRecordStore(p->rs);

  /* Will free memory used by the memfs */
  fclose(fp);

  return 0;
}

static cibyl_fops_t record_store_fops =
{
  .priv_data_size = sizeof(record_store_file_t),
  .open  = open,
  .close = close,
  .read  = NULL, /* See below*/
  .write = NULL,
  .seek  = NULL,
  .tell  = NULL,
  .flush = NULL,
};

static void __attribute__((constructor))register_fs(void)
{
  record_store_fops.read  = NOPH_Memory_fops.read;
  record_store_fops.write = NOPH_Memory_fops.write;
  record_store_fops.tell  = NOPH_Memory_fops.tell;
  record_store_fops.seek  = NOPH_Memory_fops.seek;
  record_store_fops.flush = NOPH_Memory_fops.flush;

  cibyl_fops_register("recordstore://", &record_store_fops, 0);
}
