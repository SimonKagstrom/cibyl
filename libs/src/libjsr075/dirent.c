/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      dir-operations.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Directory operations
 *
 * $Id:$
 *
 ********************************************************************/
#include <java/util.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>     /* calloc */

#include <javax/microedition/io.h>

typedef struct
{
  NOPH_FileConnection_t fc;
  NOPH_Enumeration_t it;
} fileconnection_dir_t;

static cibyl_dops_t fileconnection_dops;

static DIR *open_dir(const char *dirname)
{
  NOPH_FileConnection_t fc;
  int exception = 0;
  DIR *out;
  fileconnection_dir_t *p;

  NOPH_try(NOPH_setter_exception_handler, &exception) {
    fc = NOPH_Connector_openFileConnection(dirname);
    exception = 0;
  } NOPH_catch();
  if (exception)  /* Exception - cannot open */
    return NULL;

  out = cibyl_dir_alloc(&fileconnection_dops);
  p = (fileconnection_dir_t*)out->priv;

  p->fc = fc;
  NOPH_try(NOPH_setter_exception_handler, &exception) {
    p->it = NOPH_FileConnection_list(fc);
    exception = 0;
  } NOPH_catch();
  if (exception)  /* Exception - cannot open */
    goto clean;

  return out;

 clean:
  cibyl_dir_free(out);
  NOPH_FileConnection_close(fc);

  return NULL;
}

static int close_dir(DIR *dir)
{
  fileconnection_dir_t* p = (fileconnection_dir_t*)dir->priv;

  NOPH_FileConnection_close(p->fc);
  NOPH_delete(p->it);
  NOPH_delete(p->fc);

  return 0;
}


static int read_dir(DIR *dir, struct dirent *entry)
{
  fileconnection_dir_t* p = (fileconnection_dir_t*)dir->priv;
  NOPH_Object_t cur;
  int exception = 0;

  if (!NOPH_Enumeration_hasMoreElements(p->it))
    return -1;

  NOPH_try(NOPH_setter_exception_handler, &exception) {
    cur = NOPH_Enumeration_nextElement(p->it);
    exception = 0;
  } NOPH_catch();
  if (exception)
    return -1;

  /* Write the string to the dirent */
  NOPH_String_toCharPtr(cur, entry->d_name, 256);
  NOPH_delete(cur); /* Free the temporary object */

  return 0;
}


/* The dops structure */
static cibyl_dops_t fileconnection_dops =
{
  .keep_uri = 1,
  .priv_data_size = sizeof(fileconnection_dir_t),
  .opendir  = open_dir,
  .closedir = close_dir,
  .readdir  = read_dir,
};

static void __attribute__((constructor))register_fs(void)
{
  /* Register as default */
  cibyl_dops_register("file://", &fileconnection_dops, 1);
}
