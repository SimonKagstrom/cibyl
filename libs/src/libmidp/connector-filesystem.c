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
  char *path;
} connector_file_t;

static FILE *open(const char *path,
                  cibyl_fops_open_mode_t mode)
{
  FILE *fp;
  connector_file_t *p;
  int error = 0;

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
  NOPH_try(NOPH_setter_exception_handler, (void*)&error)
    {
      p->is_file.is = NOPH_Connector_openDataInputStream((char*)path);
    } NOPH_catch();
  if (error)
    {
      cibyl_file_free(fp);
      return NULL;
    }

  p->path = strdup(path);
  /* We cannot get the file size for the connectors */

  return fp;
}

static int close(FILE *fp)
{
  connector_file_t *p = (connector_file_t *)fp->priv;

  NOPH_InputStream_close(p->is_file.is);

  free(p->path);

  return 0;
}

FILE *NOPH_Connector_openFILE(const char *path, const char *in_mode)
{
  return open(path, cibyl_file_get_mode(in_mode));
}


/* The fops structure for connector files */
static cibyl_fops_t connector_fops =
{
  .keep_uri = 1,
  .priv_data_size = sizeof(connector_file_t),
  .open  = open,
  .close = close,
  .read  = NULL, /* Set below */
  .write = NULL, /* ... */
  .seek  = NULL,
};

static void __attribute__((constructor))register_fs(void)
{
  /* By default uses the same implementations as input streams */
  connector_fops.read  = NOPH_InputStream_fops.read;
  connector_fops.write = NOPH_InputStream_fops.write;
  connector_fops.seek  = NOPH_InputStream_fops.seek;

  /* Lots of different types are handle by the connector */
  cibyl_fops_register("file://", &connector_fops, 0); /* Can be overridden by jsr075 */
  cibyl_fops_register("http://", &connector_fops, 0);
  cibyl_fops_register("https://", &connector_fops, 0);
  cibyl_fops_register("datagram://", &connector_fops, 0);
  cibyl_fops_register("comm:", &connector_fops, 0);
  cibyl_fops_register("socket://", &connector_fops, 0);
  cibyl_fops_register("ssl://", &connector_fops, 0);
}
