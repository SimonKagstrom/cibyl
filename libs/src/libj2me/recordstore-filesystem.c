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
#include <assert.h>
#include <stdlib.h>

typedef struct
{
  FILE *memfs_fp;
  NOPH_RecordStore_t rs;
  int id;
  void (*close_helper)(FILE *fp);
} record_store_file_t;

static void exception_handler(NOPH_Exception_t ex, void *arg)
{
  int *p = (int*)arg;
  *p = 1;

  NOPH_delete(ex);
}

static void close_write(FILE *fp)
{
  record_store_file_t *p = (record_store_file_t *)fp->priv;
  void *data = NOPH_memoryFile_getDataPtr(p->memfs_fp);
  long size = ftell(p->memfs_fp);

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

  NOPH_try(exception_handler, (void*)&error)
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
static int open(FILE *fp, const char *path,
                cibyl_fops_open_mode_t mode)
{
  record_store_file_t *p = (record_store_file_t *)fp->priv;
  char *record_number = strchr(path, ':');
  char *store_name;
  void *data = NULL;
  int rs_size = 8192; /* assume mode == TRUNCATE */
  int error;

  if (!record_number)
    return -1;

  store_name = strdup(path);
  *strchr(store_name, ':') = '\0'; /* MUST exist because of above */

  /* Open the store and set the id */
  NOPH_try(exception_handler, (void*)&error)
    {
      p->rs = NOPH_RecordStore_openRecordStore(store_name,
                                               mode == READ ? 0 : 1);
    } NOPH_catch();
  if (error)
    {
      free(store_name);
      return -1;
    }
  p->id = atoi(record_number + 1);
  p->close_helper = close_write; /* assume write */
  free(store_name);

  switch (mode)
    {
    case TRUNCATE:
      /* Just allocate space without reading anything */
      break;
    case WRITE:
    case APPEND:
      /* Write or append - read into a buffer */
      if ( !(data = get_record(p->rs, p->id)) )
        return -1;
    case READ:
      /* Open and read everything into a buffer */
      if ( !(data = get_record(p->rs, p->id)) )
        return -1;
      /* Do not update on close */
      p->close_helper = close_read;
      break;
    default:
      assert(0 && "Invalid mode");
      return -1;
    }

  /* Create the memory file */
  p->memfs_fp = NOPH_memoryFile_open(data, rs_size, 1);

  if (mode == APPEND)
    fseek(p->memfs_fp, 0, SEEK_END);

  return 0;
}

static int close(FILE *fp)
{
  record_store_file_t *p = (record_store_file_t *)fp->priv;

  /* Call the helper to (sometimes) save the record data */
  p->close_helper(fp);
  /* Will free memory used by the memfs */
  fclose(p->memfs_fp);
  NOPH_RecordStore_closeRecordStore(p->rs);

  return 0;
}

/* All these are basically "inherited" from the memory file system */
static int seek(FILE *fp, long offset, int whence)
{
  record_store_file_t *p = (record_store_file_t *)fp->priv;

  return fseek(p->memfs_fp, offset, whence);
}

static long tell(FILE *fp)
{
  record_store_file_t *p = (record_store_file_t *)fp->priv;

  return ftell(p->memfs_fp);
}

static size_t read(FILE *fp, void *ptr, size_t in_size)
{
  record_store_file_t *p = (record_store_file_t *)fp->priv;

  return fread(ptr, in_size, 1, p->memfs_fp);
}

static size_t write(FILE *fp, const void *ptr, size_t in_size)
{
  record_store_file_t *p = (record_store_file_t *)fp->priv;

  return fwrite(ptr, in_size, 1, p->memfs_fp);
}

static int eof(FILE *fp)
{
  record_store_file_t *p = (record_store_file_t *)fp->priv;

  return feof(p->memfs_fp);
}



static cibyl_fops_t record_store_fops =
{
  .uri = "recordstore://",
  .priv_data_size = sizeof(record_store_file_t),
  .open = open,
  .close = close,
  .read = read,
  .write = write,
  .seek = seek,
  .tell = tell,
  .eof = eof,
};

static void __attribute__((constructor))register_fs(void)
{
  cibyl_register_fops(&record_store_fops, 1);
}
