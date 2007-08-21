#include <string.h>
#include <stdlib.h>

#include <cibyl-fileops.h>
#include <javax/microedition/lcdui/game.h>
#include <javax/microedition/io.h>
#include <java/io.h>
#include <java/lang.h>

static cibyl_fops_t connector_fops;

typedef struct
{
  NOPH_InputStream_file_t is_file;
  NOPH_FileConnection_t fc;
  char *path;
} fileconnection_file_t;

static FILE *open_file(const char *path, cibyl_fops_open_mode_t mode)
{
  NOPH_FileConnection_t fc;
  FILE *fp;
  fileconnection_file_t *p;
  int error = 0;

  /* Handle the write case by simply returning a FILE for the output stream */
  if (mode == WRITE || mode == APPEND || mode == READ_TRUNCATE)
    {
      NOPH_OutputStream_t os;
      FILE *out;

      fc = NOPH_Connector_openFileConnection_mode(path, NOPH_Connector_READ_WRITE);

      /* Create and maybe truncate the file */
      NOPH_try(NOPH_setter_exception_handler, (void*)&error)
        {
          if (!NOPH_FileConnection_exists(fc))
            NOPH_FileConnection_create(fc);
          if (mode == READ_TRUNCATE)
            NOPH_FileConnection_truncate(fc, 0);
        } NOPH_catch();
      if (error)
        {
          NOPH_delete(fc);
          return NULL;
        }

      os = NOPH_FileConnection_openDataOutputStream(fc); /* Can throw stuff */

      NOPH_delete(fc);
      if (error)
        {
          NOPH_OutputStream_close(os);
          return NULL;
        }
      out = NOPH_OutputStream_createFILE(os);

      if (!out)
        {
          NOPH_OutputStream_close(os);
          return NULL;
        }

      return out;
    }

  fp = cibyl_file_alloc(&connector_fops);
  p = (fileconnection_file_t *)fp->priv;

  if (mode != READ)
    NOPH_throw(NOPH_Exception_new_string("Opening a connector stream with invalid mode"));

  /* Try to open the connector stream. */
  NOPH_try(NOPH_setter_exception_handler, (void*)&error)
    {
      fc = NOPH_Connector_openFileConnection_mode(path, NOPH_Connector_READ);
      p->is_file.is = NOPH_FileConnection_openDataInputStream(fc);
    } NOPH_catch();
  if (error)
    {
      cibyl_file_free(fp);
      return NULL;
    }

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

  if (real_skip != skip)
    NOPH_throw(NOPH_Exception_new());
}


/* The fops structure for connector files */
static cibyl_fops_t connector_fops =
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

static void __attribute__((constructor))register_fs(void)
{
  /* By default uses the same implementations as input streams */
  connector_fops.read  = NOPH_InputStream_fops.read;
  connector_fops.write = NOPH_InputStream_fops.write;

  cibyl_fops_register("file://", &connector_fops, 0);
}
