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

int test_seek_cur_read(FILE *fp)
{
  char buf[4] = {0};
  int n;

  fseek(fp, 2, SEEK_CUR);
  n = fread((void*)buf, sizeof(char), 4, fp);
  if (n != 4)
    return n;
  else if (buf[0] != 'K' ||
	   buf[1] != 'L' ||
	   buf[2] != 'M' ||
	   buf[3] != 'N')
    return -1;

  return n;
}

int test_seek_end_read(FILE *fp)
{
  char buf[4] = {0};
  int n;

  fseek(fp, 4, SEEK_END);
  n = fread((void*)buf, sizeof(char), 4, fp);
  if (n != 4)
    return n;
  else if (buf[0] != 'N' ||
	   buf[1] != 'O' ||
	   buf[2] != 'P' ||
	   buf[3] != 'Q')
    return -1;

  return n;
}

int test_seek_end_read_end(FILE *fp)
{
  char buf[4] = {0};
  int n;

  fseek(fp, 3, SEEK_END);
  n = fread((void*)buf, sizeof(char), 4, fp);
  if (n != 4)
    return n;
  else if (buf[0] != 'O' ||
	   buf[1] != 'P' ||
	   buf[2] != 'Q')
    return -1;

  return n;
}

static void parse_return_val(const char *op, const char *path, int ret, int xret)
{
  if (ret < 0)
    FAIL("%s: %s\n", op, path);
  else if (ret != xret)
    FAIL("%s: %s size (%d != %d)\n", op, path, ret, xret);
  else
    PASS("%s: %s sucessful\n", op, path);
}

/* Pass an opened file for the test to run */
void fs_read_test(FILE *fp, const char *op, const char *path)
{
  /* Test a plain read */
  parse_return_val(op, path,
                   test_read(fp), 4);
  /* Test a seek from the start and read */
  parse_return_val(op, path,
                   test_seek_read(fp), 4);
  /* Test seek ahead and read */
  parse_return_val(op, path,
                   test_seek_cur_read(fp), 4);
  /* Test seek from the end */
  parse_return_val(op, path,
                   test_seek_end_read(fp), 4);
  /* Test seek from the end and read less than what is left  */
  parse_return_val(op, path,
                   test_seek_end_read_end(fp), 3);
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
  NOPH_Throwable_printStackTrace(exception);
  NOPH_delete(exception);
}

#define ROOT_PATH "file:///root/"
void file_operations_run(void)
{
  const char *root = ROOT_PATH;
  const char *path = ROOT_PATH"/a";
  FILE *fp;
  int exception = 0;

  fp = fopen(path, "w");
  exception = 0;
  if (fp)
    {
      PASS("Connector open %s\n", path);

      parse_return_val("Connector write", path,
		       test_write(fp), strlen(test_str) );
      fclose(fp);
    }
  else
    FAIL("Connector open write %s\n", path);

  fp = fopen(path, "r");
  if (fp)
    {
      PASS("Connector open %s\n", path);

      fs_read_test(fp, "Connector", path);
      fclose(fp);
    }
  else
    FAIL("Connector open read %s\n", path);

  path = "/b"; /* Resource */
  fp = fopen(path, "r");
  if (fp)
    {
      PASS("Resource read %s\n", path);

      fs_read_test(fp, "Resource", path);
      fclose(fp);
    }
  else
    FAIL("Resource open %s\n", path);

  /* Test directory stuff */
  if (dir_open(root))
    PASS("Dirlist %s\n", root);
  else
    FAIL("Dirlist %s\n", root);
}
