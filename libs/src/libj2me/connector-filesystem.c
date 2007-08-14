#include <string.h>
#include <stdlib.h>

#include <cibyl-fileops.h>
#include <javax/microedition/lcdui/game.h>
#include <javax/microedition/io.h>
#include <java/io.h>
#include <java/lang.h>

#define min(x,y) ( (x) < (y) ? (x) : (y) )

static cibyl_fops_t connector_fops;

typedef struct
{
  NOPH_InputStream_file_t is_file;
  char *path;
} connector_file_t;

static void exception_handler(NOPH_Exception_t ex, void *arg)
{
  int *p = (int*)arg;
  *p = 1;

  NOPH_delete(ex);
}

FILE *NOPH_FileConnection_openFILE(const char *path, const char *in_mode)
{
  cibyl_fops_open_mode_t mode = cibyl_file_get_mode(in_mode);
  NOPH_FileConnection_t fc;
  FILE *fp;
  connector_file_t *p;
  int error = 0;

  /* Handle the write case by simply returning a FILE for the output stream */
  if (mode == WRITE || mode == APPEND || mode == READ_TRUNCATE)
    {
      NOPH_OutputStream_t os;
      FILE *out;

      fc = NOPH_Connector_openFileConnection_mode(path, NOPH_Connector_WRITE);
      os = NOPH_FileConnection_openDataOutputStream(fc); /* Can throw stuff */

      /* Create and maybe truncate the file */
      NOPH_try(exception_handler, (void*)&error)
        {
          if (!NOPH_FileConnection_exists(fc))
            NOPH_FileConnection_create(fc);
          if (mode == READ_TRUNCATE)
            NOPH_FileConnection_truncate(fc, 0);
        } NOPH_catch();
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
  p = (connector_file_t *)fp->priv;

  if (mode != READ)
    NOPH_throw(NOPH_Exception_new_string("Opening a connector stream with invalid mode"));
  /* Try to open the connector stream. */
  NOPH_try(exception_handler, (void*)&error)
    {
      fc = NOPH_Connector_openFileConnection_mode(p->path, NOPH_Connector_READ);
      p->is_file.is = NOPH_FileConnection_openDataInputStream(fc);
    } NOPH_catch();
  if (error)
    {
      cibyl_file_free(fp);
      return NULL;
    }

  p->path = strdup(path);
  p->is_file.eof = 0;
  p->is_file.is_fp = 0;

  return fp;
}

static FILE *open(const char *path,
                  cibyl_fops_open_mode_t mode)
{
  FILE *fp;
  connector_file_t *p;
  int error = 0;

  printf("Conn: trying to open %s:%d!\n", path, mode);

  /* Handle the write case by simply returning a FILE for the output stream */
  if (mode == WRITE || mode == APPEND)
    {
      NOPH_OutputStream_t os = NOPH_Connector_openDataOutputStream((char*)path); /* Can throw stuff */
      FILE *out = NOPH_OutputStream_createFILE(os);

      if (!out)
        {
          NOPH_OutputStream_close(os);
          return NULL;
        }

      return out;
    }

  fp = cibyl_file_alloc(&connector_fops);
  p = (connector_file_t *)fp->priv;

  if (mode != READ)
    NOPH_throw(NOPH_Exception_new_string("Opening a connector stream with invalid mode"));
  /* Try to open the connector stream. */
  NOPH_try(exception_handler, (void*)&error)
    {
      p->is_file.is = NOPH_Connector_openDataInputStream((char*)path);
    } NOPH_catch();
  if (error)
    {
      cibyl_file_free(fp);
      return NULL;
    }

  p->path = strdup(path);
  p->is_file.eof = 0;
  p->is_file.is_fp = 0;

  return fp;
}

static int close(FILE *fp)
{
  connector_file_t *p = (connector_file_t *)fp->priv;

  NOPH_InputStream_close(p->is_file.is);

  free(p->path);

  return 0;
}

static void reset(FILE *fp)
{
  connector_file_t *p = (connector_file_t *)fp->priv;
  int error = 0;

  NOPH_try(exception_handler, (void*)&error)
    {
      NOPH_InputStream_reset(p->is_file.is);
    } NOPH_catch();
  if (error)
    {
      NOPH_FileConnection_t fc;

      /* FileConnections - reset by reopening */
      NOPH_InputStream_close(p->is_file.is);
      fc = NOPH_Connector_openFileConnection(p->path);
      p->is_file.is = NOPH_FileConnection_openInputStream(fc);
    }
}

static int seek(FILE *fp, long offset, int whence)
{
  connector_file_t *p = (connector_file_t *)fp->priv;
  int skip = offset;
  int avail;
  int error = 0;

  /* Only inputstreams can support seeking */
  switch (whence)
    {
    case SEEK_SET:
      p->is_file.is_fp = 0;
      reset(fp);
      break;
    case SEEK_END:
      p->is_file.is_fp = 0;
      avail = NOPH_InputStream_available(p->is_file.is);
      skip = avail - offset;
      reset(fp);
      break;
    case SEEK_CUR:
      /* Do nothing (fallthrough) */
    default:
      break;
    }
  NOPH_try(exception_handler, (void*)&error)
    {
      p->is_file.is_fp += NOPH_InputStream_skip(p->is_file.is, skip);
    } NOPH_catch();
  if (error)
    return -1;

  return 0;
}


/* The fops structure for connector files */
static cibyl_fops_t connector_fops =
{
  .uri = "file://",
  .keep_uri = 1,
  .priv_data_size = sizeof(connector_file_t),
  .open  = open,
  .close = close,
  .read  = NULL, /* Set below */
  .write = NULL, /* Set below */
  .seek  = seek,
  .tell  = NULL, /* Set below */
  .eof   = NULL, /* Set below */
};

static void __attribute__((constructor))register_fs(void)
{
  /* By default uses the same implementations as input streams */
  connector_fops.read  = NOPH_InputStream_fops.read;
  connector_fops.write = NOPH_InputStream_fops.write;
  connector_fops.tell  = NOPH_InputStream_fops.tell;
  connector_fops.eof   = NOPH_InputStream_fops.eof;

  cibyl_fops_register(&connector_fops, 0);
}
