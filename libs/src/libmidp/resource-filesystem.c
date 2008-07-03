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

static FILE *open(const char *path,
                  cibyl_fops_open_mode_t mode)
{
  FILE *fp;
  NOPH_InputStream_t is = 0;
  resource_file_t *p;
  int error = 0;

  /* Try to open the resource stream. They are always read-only, so
   * just ignore the mode
   */
  NOPH_try(NOPH_setter_exception_handler, (void*)&error)
    {
      NOPH_GameCanvas_t gc = NOPH_GameCanvas_get();
      NOPH_Class_t cl = NOPH_Object_getClass(gc);
      is = NOPH_Class_getResourceAsStream(cl, (char*)path);
    } NOPH_catch();
  if (error || is == 0)
    return NULL;

  fp = cibyl_file_alloc(&resource_fops);
  p = (resource_file_t *)fp->priv;
  p->is_file.is = is;
  fp->file_size = NOPH_InputStream_available(is);

  //  if (NOPH_InputStream_markSupported(p->is_file.is))
  //  NOPH_InputStream_mark(p->is_file.is, 0x7fffffff);

  return fp;
}


/* The fops structure for resource files */
static cibyl_fops_t resource_fops =
{
  .priv_data_size = sizeof(resource_file_t),
  .open  = open,
  .close = NULL, /* Set below */
  .read  = NULL, /* ... */
  .write = NULL,
  .seek  = NULL,
};

static void __attribute__((constructor))resource_register_fs(void)
{
  resource_fops.close = NOPH_InputStream_fops.close;
  resource_fops.read  = NOPH_InputStream_fops.read;
  resource_fops.write = NOPH_InputStream_fops.write;
  resource_fops.seek  = NOPH_InputStream_fops.seek;
  resource_fops.flush = NOPH_InputStream_fops.flush;

  cibyl_fops_register("resource://", &resource_fops, 1);
}
