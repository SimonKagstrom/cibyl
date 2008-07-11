#include <string.h>
#include <stdlib.h>

#include <cibyl-fileops.h>
#include <java/io.h>
#include <java/lang.h>

static cibyl_fops_t file_input_fops;
static cibyl_fops_t file_output_fops;

typedef struct
{
  NOPH_FileInputStream_t is_file;
} file_inputfile_t;

typedef struct
{
  NOPH_FileOutputStream_t os_file;
} file_outputfile_t;

static FILE *open_file(const char *path, cibyl_fops_open_mode_t mode)
{
  FILE *fp;
  file_inputfile_t *p;
  int error = 0;

  /* Handle the write case by simply returning a FILE for the output stream */
  if (mode == WRITE || mode == APPEND || mode == READ_TRUNCATE)
    {
      file_outputfile_t *p;
      long offset = 0;

      fp = cibyl_file_alloc(&file_output_fops);
      p = (file_outputfile_t*)fp->priv;
      fp->file_size = 0;

      /* Create and maybe truncate the file */
      NOPH_try(NOPH_setter_exception_handler, (void*)&error)
        {
          p->os_file = NOPH_FileOutputStream_new(path);
          error = 0;
        } NOPH_catch();
      if (error)
        {
          cibyl_file_free(fp);
          return NULL;
        }

      return fp;
    }

  fp = cibyl_file_alloc(&file_input_fops);
  p = (file_inputfile_t *)fp->priv;

  if (mode != READ)
    NOPH_throw(NOPH_Exception_new_string("Opening a connector stream with invalid mode"));

  /* Try to open the connector stream. */
  NOPH_try(NOPH_setter_exception_handler, (void*)&error)
    {
      p->is_file = NOPH_FileInputStream_new(path);
      error = 0;
    } NOPH_catch();
  if (error)
    {
      cibyl_file_free(fp);
      return NULL;
    }

  return fp;
}

static FILE *open(const char *path,
                  cibyl_fops_open_mode_t mode)
{
  return open_file(path, mode);
}

/* The fops structure for connector files */
static cibyl_fops_t file_input_fops =
{
  .priority = 1, /* Higher priority than connections */
  .priv_data_size = sizeof(file_inputfile_t),
  .open  = open,
  .close = NULL,
  .read  = NULL, /* Set below */
  .write = NULL, /* ... */
  .seek  = NULL,
};

static cibyl_fops_t file_output_fops =
{
  .priority = 1, /* Higher priority than connections */
  .priv_data_size = sizeof(file_inputfile_t),
  .open  = open,
  .close = NULL,
  .read  = NULL, /* Not applicable */
  .write = NULL, /* Set below */
  .seek  = NULL, /* Not applicable */
};

static void __attribute__((constructor))fileconnection_register_fs(void)
{
  /* By default uses the same implementations as input streams */
  file_input_fops.read  = NOPH_InputStream_fops.read;
  file_input_fops.seek = NOPH_InputStream_fops.seek;
  file_input_fops.close= NOPH_InputStream_fops.close;
  file_output_fops.close = NOPH_OutputStream_fops.close;
  file_input_fops.write = NOPH_InputStream_fops.write;
  file_output_fops.write = NOPH_OutputStream_fops.write;

  /* file:// and default is allocated */
  cibyl_fops_register("file://", &file_input_fops, 1);
}
