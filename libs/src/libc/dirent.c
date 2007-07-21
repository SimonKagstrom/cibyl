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

static void handler(NOPH_Exception_t exception, void *arg)
{
  *(int*)arg = 1;
  NOPH_delete(exception);
}

DIR *opendir(const char *dirname)
{
  NOPH_FileConnection_t fc;
  int exception = 0;
  DIR *out;

  NOPH_try(handler, &exception) {
    fc = NOPH_Connector_openFileConnection(dirname);
    exception = 0;
  } NOPH_catch();
  if (exception)  /* Exception - cannot open */
    return NULL;

  if ( !(out = calloc(1, sizeof(DIR))) )
    goto clean_1;

  out->fc = fc;
  NOPH_try(handler, &exception) {
    out->it = NOPH_FileConnection_list(fc);
    exception = 0;
  } NOPH_catch();
  if (exception)  /* Exception - cannot open */
    goto clean_2;

  return out;

 clean_2:
  free(out);
 clean_1:
  NOPH_FileConnection_close(fc);

  return NULL;
}

int closedir(DIR *dir)
{
  NOPH_delete(dir->it);
  NOPH_FileConnection_close(dir->fc);

  return 0;
}


int readdir_r(DIR *dir, struct dirent *entry,
              struct dirent **result)
{
  NOPH_Object_t cur;
  int exception = 0;

  if (!NOPH_Enumeration_hasMoreElements(dir->it))
    goto cleanup;

  NOPH_try(handler, &exception) {
    cur = NOPH_Enumeration_nextElement(dir->it);
    exception = 0;
  } NOPH_catch();
  if (exception)
    goto cleanup;

  /* Write the string to the dirent */
  NOPH_String_toCharPtr(cur, entry->d_name, 256);
  NOPH_delete(cur); /* Free the temporary object */
  *result = entry;

  return 0;
 cleanup:
  NOPH_FileConnection_close(dir->fc);
  NOPH_delete(dir->it);
  *result = NULL;
  return -1;
}

struct dirent *readdir(DIR *dir)
{
  static struct dirent global_dirent;
  struct dirent *out;

  if ( readdir_r(dir, &global_dirent, &out) != 0 )
    return NULL;

  return &global_dirent;
}
