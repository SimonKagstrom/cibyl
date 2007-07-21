/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      file-operations.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Cibyl file operation tests
 *
 * $Id: file-operations.c 13719 2007-02-19 16:29:51Z ska $
 *
 ********************************************************************/
#include <test.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <javax/microedition/io.h>

static const char *test_str = "ABCDEFGHIJKLMNOPQ";

int test_write(FILE *fp)
{
  return fwrite((const void*)test_str, sizeof(char), strlen(test_str), fp);
}

int test_read(FILE *fp)
{
  char buf[4] = {0};
  int n;

  n = fread((void*)buf, sizeof(char), 4, fp);
  if (n != 4)
    return n;
  else if (buf[0] != 'A' ||
	   buf[1] != 'B' ||
	   buf[2] != 'C' ||
	   buf[3] != 'D')
    return -1;

  return n;
}

int test_seek_read(FILE *fp)
{
  char buf[4] = {0};
  int n;

  fseek(fp, 4, SEEK_SET);
  n = fread((void*)buf, sizeof(char), 4, fp);
  if (n != 4)
    return n;
  else if (buf[0] != 'E' ||
	   buf[1] != 'F' ||
	   buf[2] != 'G' ||
	   buf[3] != 'H')
    return -1;

  return n;
}

static void parse_return_val(const char *op, const char *path, int ret, int xret)
{
  if (ret < 0)
    FAIL("%s: Incorrect operation %s\n", op, path);
  else if (ret != xret)
    FAIL("%s: %s with incorrect size (%d != %d)\n", op, path, ret, xret);
  else
    PASS("%s: %s sucessful\n", op, path);
}

static int dir_open(const char *dir)
{
  DIR *d = opendir(dir);
  struct dirent *de;
  int ret = 0;

  if (!d)
    return ret;

  for (de = readdir(d);
       de;
       de = readdir(d))
    {
      if (strcmp(de->d_name, "a") == 0)
        ret = 1;
    }
  closedir(d);
  return ret;
}


static void handler_file_io(NOPH_Exception_t exception, void *arg)
{
  *(int*)arg = 1;
  NOPH_delete(exception);
}

#define ROOT_PATH "file:///root/"
void file_operations_run(void)
{
  const char *root = ROOT_PATH;
  const char *path = ROOT_PATH"/a";
  FILE *fp;
  int exception = 0;

  NOPH_try(handler_file_io, (void*)&exception) {
    fp = NOPH_Connector_openFILEOutputStream(path);
    exception = 0;
  } NOPH_catch();
  if (fp && exception == 0)
    {
      PASS("Connector write %s\n", path);

      parse_return_val("Connector write", path,
		       test_write(fp), strlen(test_str) );
      fclose(fp);
    }
  else
    FAIL("Connector write %s\n", path);

  NOPH_try(handler_file_io, (void*)&exception) {
    fp = NOPH_Connector_openFILEInputStream(path);
    exception = 0;
  } NOPH_catch();
  if (fp && exception == 0)
    {
      PASS("Connector read %s\n", path);

      parse_return_val("Connector read", path,
		       test_read(fp), 4 );
      fclose(fp);
    }
  else
    FAIL("Connector read %s\n", path);

  /* Open already tested */
  NOPH_try(handler_file_io, (void*)&exception) {
    fp = NOPH_Connector_openFILEInputStream(path);
    exception = 0;
  } NOPH_catch();
  if (fp && exception == 0)
    {
      parse_return_val("Connector read+seek", path,
		       test_seek_read(fp), 4 );
      fclose(fp);
    }

  path = "/b";
  fp = fopen(path, "r");
  if (fp)
    {
      fseek(fp, 0, SEEK_SET);
      PASS("Resource read %s\n", path);

      parse_return_val("Resource read", path,
		       test_read(fp), 4 );
      fclose(fp);
    }
  else
    FAIL("Resource read %s\n", path);

  /* Open already tested */
  fp = fopen(path, "r");
  if (fp)
    {
      parse_return_val("Resource read+seek", path,
		       test_seek_read(fp), 4 );
      fclose(fp);
    }

  if (dir_open(root))
    PASS("Dirlist %s\n", root);
  else
    FAIL("Dirlist %s\n", root);
}
