#include <cibyl-fileops.h>
#include <javax/microedition/lcdui/game.h>
#include <java/io.h>
#include <java/lang.h>

#define min(x,y) ( (x) < (y) ? (x) : (y) )

static cibyl_fops_t resource_fops;

typedef struct
{
  NOPH_InputStream_file_t is_file;
} resource_file_t;

static void exception_handler(NOPH_Exception_t ex, void *arg)
{
  int *p = (int*)arg;
  *p = 1;

  NOPH_delete(ex);
}

static FILE *open(const char *path,
                  cibyl_fops_open_mode_t mode)
{
  FILE *fp;
  NOPH_GameCanvas_t gc = NOPH_GameCanvas_get();
  NOPH_InputStream_t is;
  resource_file_t *p;
  int error;

  /* Try to open the resource stream. They are always read-only, so
   * just ignore the mode
   */
  NOPH_try(exception_handler, (void*)&error)
    {
      is = NOPH_Class_getResourceAsStream(gc, (char*)path);
    } NOPH_catch();
  if (error)
    return NULL;

  fp = cibyl_file_alloc(&resource_fops);
  p = (resource_file_t *)fp->priv;
  p->is_file.is_fp = 0;
  p->is_file.eof = 0;
  p->is_file.is = is;

  return fp;
}


/* The fops structure for resource files */
static cibyl_fops_t resource_fops =
{
  .uri = "resource://",
  .priv_data_size = sizeof(resource_file_t),
  .open  = open,
  .close = NULL, /* Set below */
  .read  = NULL, /* ... */
  .write = NULL,
  .seek  = NULL,
  .tell  = NULL,
  .eof   = NULL,
};

static void __attribute__((constructor))register_fs(void)
{
  resource_fops.close = NOPH_InputStream_fops.close;
  resource_fops.read  = NOPH_InputStream_fops.read;
  resource_fops.write = NOPH_InputStream_fops.write;
  resource_fops.seek  = NOPH_InputStream_fops.seek;
  resource_fops.tell  = NOPH_InputStream_fops.tell;
  resource_fops.eof   = NOPH_InputStream_fops.eof;
  resource_fops.flush = NOPH_InputStream_fops.flush;

  cibyl_register_fops(&resource_fops, 1);
}
