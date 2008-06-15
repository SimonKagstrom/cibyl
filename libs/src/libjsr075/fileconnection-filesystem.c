#include <string.h>
#include <stdlib.h>

#include <cibyl-fileops.h>
#include <javax/microedition/lcdui/game.h>
#include <javax/microedition/io.h>
#include <java/io.h>
#include <java/lang.h>

static cibyl_fops_t fileconnection_input_fops;
static cibyl_fops_t fileconnection_output_fops;

typedef struct
{
  NOPH_InputStream_file_t is_file;
  NOPH_FileConnection_t fc;
  char *path;
} fileconnection_file_t;

typedef struct
{
  NOPH_OutputStream_file_t os_file;
  NOPH_FileConnection_t fc;
} fileconnection_outputfile_t;

static FILE *open_file(const char *path, cibyl_fops_open_mode_t mode)
{
  NOPH_FileConnection_t fc;
  FILE *fp;
  fileconnection_file_t *p;
  int error = 0;

  /* Handle the write case by simply returning a FILE for the output stream */
  if (mode == WRITE || mode == APPEND || mode == READ_TRUNCATE)
    {
      fileconnection_outputfile_t *p;
      long offset = 0;
      int fc_mode = NOPH_Connector_READ_WRITE;

      fc = NOPH_Connector_openFileConnection_mode(path, fc_mode);

      /* Create and maybe truncate the file */
      //      NOPH_try(NOPH_setter_exception_handler, (void*)&error)
        {
          if (!NOPH_FileConnection_exists(fc))
            NOPH_FileConnection_create(fc);
	  else if ((mode == READ_TRUNCATE) || (mode == WRITE))
            NOPH_FileConnection_truncate(fc, 0);
          error = 0;
        } //NOPH_catch();
      if (error)
        {
          printf("e1\n");

          NOPH_delete(fc);
          return NULL;
        }

      fp = cibyl_file_alloc(&fileconnection_output_fops);
      p = (fileconnection_outputfile_t*)fp->priv;
      fp->file_size = NOPH_FileConnection_fileSize(fc);

      if (mode == APPEND) {
        offset = fp->file_size;
        fp->vfptr = fp->fptr = fp->file_size;
      }
      //      NOPH_try(NOPH_setter_exception_handler, (void*)&error)
        {
          p->os_file.os = NOPH_FileConnection_openOutputStream(fc, offset); /* Can throw stuff */
        }// NOPH_catch();
      if (error)
        {
          printf("e2\n");
          cibyl_file_free(fp);
          NOPH_delete(fc);
          return NULL;
        }

      NOPH_delete(fc);
      if (error)
        {
          printf("e3\n");
          NOPH_OutputStream_close(p->os_file.os);
          return NULL;
        }

      return fp;
    }

  fp = cibyl_file_alloc(&fileconnection_input_fops);
  p = (fileconnection_file_t *)fp->priv;

  if (mode != READ)
    NOPH_throw(NOPH_Exception_new_string("Opening a connector stream with invalid mode"));

  /* Try to open the connector stream. */
  //  NOPH_try(NOPH_setter_exception_handler, (void*)&error)
    {
      fc = NOPH_Connector_openFileConnection_mode(path, NOPH_Connector_READ);
      p->is_file.is = NOPH_FileConnection_openDataInputStream(fc);
    }// NOPH_catch();
  if (error)
    {
      printf("e4\n");
      cibyl_file_free(fp);
      return NULL;
    }

  if (NOPH_InputStream_markSupported(p->is_file.is))
    NOPH_InputStream_mark(p->is_file.is, 0x7fffffff);

  p->path = strdup(path);
  p->fc = fc;

  fp->file_size = NOPH_FileConnection_fileSize(fc);

  return fp;
}

FILE *NOPH_FileConnection_openFILE(const char *path, const char *in_mode)
{
  cibyl_fops_open_mode_t mode = cibyl_file_get_mode(in_mode);

  return open_file(path, mode);
}

static FILE *open(const char *path,
                  cibyl_fops_open_mode_t mode)
{
  return open_file(path, mode);
}

static int close(FILE *fp)
{
  fileconnection_file_t *p = (fileconnection_file_t *)fp->priv;

  NOPH_InputStream_close(p->is_file.is);
  NOPH_delete(p->fc);

  free(p->path);

  return 0;
}

static int close_output(FILE *fp)
{
  fileconnection_outputfile_t *p = (fileconnection_outputfile_t *)fp->priv;

  NOPH_OutputStream_close(p->os_file.os);
  NOPH_delete(p->fc);

  return 0;
}

static void reset(FILE *fp)
{
  fileconnection_file_t *p = (fileconnection_file_t *)fp->priv;

  /* FileConnections - reset by reopening */
  NOPH_InputStream_close(p->is_file.is);
  NOPH_delete(p->is_file.is);
  NOPH_delete(p->fc);

  p->fc = NOPH_Connector_openFileConnection(p->path); /* Read is OK */
  p->is_file.is = NOPH_FileConnection_openInputStream(p->fc);
}

static void seek(FILE *fp, long offset)
{
  fileconnection_file_t *p = (fileconnection_file_t *)fp->priv;
  int skip = offset;
  int real_skip;

  if (offset < 0)
    {
      reset(fp);
      skip = fp->vfptr;
    }
  real_skip = NOPH_InputStream_skip(p->is_file.is, skip); /* Might throw */

#if 0
  /* We'd like to do this, but some SE phones seem to return bogus
   * values from InputStream.skip(). Shame on you SE!
   */
  if (real_skip != skip)
    NOPH_throw(NOPH_Exception_new_string("Real skip != skip: %d vs %d\n", real_skip, skip));
#endif
}


/* The fops structure for connector files */
static cibyl_fops_t fileconnection_input_fops =
{
  .keep_uri = 1,
  .priority = 1, /* Higher priority than connections */
  .priv_data_size = sizeof(fileconnection_file_t),
  .open  = open,
  .close = close,
  .read  = NULL, /* Set below */
  .write = NULL, /* ... */
  .seek  = seek,
};

static cibyl_fops_t fileconnection_output_fops =
{
  .keep_uri = 1,
  .priority = 1, /* Higher priority than connections */
  .priv_data_size = sizeof(fileconnection_file_t),
  .open  = open,
  .close = close_output,
  .read  = NULL, /* Not applicable */
  .write = NULL, /* Set below */
  .seek  = NULL, /* Not applicable */
};

static void __attribute__((constructor))fileconnection_register_fs(void)
{
  /* By default uses the same implementations as input streams */
  fileconnection_input_fops.read  = NOPH_InputStream_fops.read;
  fileconnection_input_fops.write = NOPH_InputStream_fops.write;
  fileconnection_output_fops.write = NOPH_OutputStream_fops.write;

  /* Only one need to be registered */
  cibyl_fops_register("file://", &fileconnection_input_fops, 0);
}
