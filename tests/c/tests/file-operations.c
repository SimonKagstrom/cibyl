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
#ifdef HOST
void file_operations_run(void)
{
}
#else
#include <test.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h> /* mkdir */
#include <dirent.h>
#include <unistd.h> /* rmdir */
#include <javax/microedition/io.h>
#include <cibyl-memoryfs.h>

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

  fseek(fp, -4, SEEK_END);
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

  fseek(fp, -3, SEEK_END);
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
  char buf[80];

  snprintf(buf, 80, "%s read", op);

  /* Test a plain read */
  parse_return_val(buf, path,
                   test_read(fp), 4);
  /* Test a seek from the start and read */
  parse_return_val(buf, path,
                   test_seek_read(fp), 4);
  /* Test seek ahead and read */
  parse_return_val(buf, path,
                   test_seek_cur_read(fp), 4);
  /* Test seek from the end */
  parse_return_val(buf, path,
                   test_seek_end_read(fp), 4);
  /* Test seek from the end and read less than what is left  */
  parse_return_val(buf, path,
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
      if (strcmp(de->d_name, "cibyl_a") == 0)
        ret = 1;
    }
  closedir(d);
  return ret;
}

static void one_test_read(FILE *fp, const char *path, const char *name)
{
  long file_size;

  if (fp)
    {
      fseek(fp, 0, SEEK_END);
      file_size = ftell(fp);
      fseek(fp, 0, SEEK_SET);

      if (file_size != 17)
        FAIL("%s file size: %d != 17", name, file_size);
      else
        PASS("%s file size: %d == 17", name, file_size);

      PASS("%s open %s", name, path);

      fs_read_test(fp, name, path);
      fclose(fp);
    }
  else
    FAIL("%s open %s", name, path);
}

static void one_test_write(FILE *fp, const char *path, const char *name)
{
  if (fp)
    {
      char buf[80];

      snprintf(buf, 80, "%s write", name);
      PASS("%s open %s\n", name, path);

      parse_return_val(buf, path,
		       test_write(fp), strlen(test_str) );
      fclose(fp);
    }
  else
    FAIL("%s open write %s\n", name, path);
}

extern char *fs_root;

void file_operations_run(void)
{
  char buf[128];
  char root[128];
  char *path;
  NOPH_FileConnection_t fc;
  int error = 0;
  FILE *fp;
  int r;

  snprintf(buf, 128, "file:///%s/cibyl_a", fs_root);
  path = buf;

  one_test_write(fopen(path, "w"), path, "FileConnection");
  one_test_read(fopen(path, "r"), path, "FileConnection");
  one_test_read(NOPH_MemoryFile_openIndirect(path, "r"), path, "MemoryFile"); /* Memory file for the file connection */

  path = "/b"; /* Resource */
  one_test_read(fopen(path, "r"), path, "Resource");
  one_test_read(NOPH_MemoryFile_openIndirect(path, "r"), path, "MemoryFile"); /* Memory file for resources */

  path = "recordstore://tjoho:3"; /* Recordstore */
  one_test_write(fopen(path, "rw"), path, "Recordstore"); /* Try to open a file in read-write mode which does not exist */
  one_test_read(fopen(path, "r"), path, "Recordstore");
  one_test_read(NOPH_MemoryFile_openIndirect(path, "r"), path, "MemoryFile"); /* Memory file for the recordstore */

  /* Reads of record stores which should fail */
  if ( (fp = fopen("recordstore://tjoho:4", "r")) )
    {
      fclose(fp);
      FAIL("Opening non-existent record store %d", 4);
    }
  else
    PASS("Opening non-existent record store %d", 4);
  if ( (fp = fopen("recordstore://tjoho:2", "r")) )
    {
      fclose(fp);
      FAIL("Opening non-existent record store %d", 2);
    }
  else
    PASS("Opening non-existent record store %d", 2);

  path = "recordstore://tjoho:2"; /* Recordstore, add a new one (previously invalid) */
  one_test_write(fopen(path, "w+"), path, "Recordstore");
  one_test_read(fopen(path, "r"), path, "Resource");

  NOPH_try(NOPH_setter_exception_handler, (void*)&error) {
    NOPH_RecordStore_deleteRecordStore("tjoho");
  } NOPH_catch();
  if (error)
    FAIL("Deleting %s", path);
  else
    PASS("Deleting %s", path);

  /* Test directory stuff */
  snprintf(root, 128, "file:///%s", fs_root);
  if (dir_open(root))
    PASS("Dirlist %s\n", root);
  else
    FAIL("Dirlist %s\n", root);

  /* Delete the file again */
  path = buf;
  error = 0;
  NOPH_try(NOPH_setter_exception_handler, (void*)&error) {
    fc = NOPH_Connector_openFileConnection_mode(path, NOPH_Connector_WRITE);
    NOPH_FileConnection_delete(fc);
  } NOPH_catch();
  if (error)
    FAIL("Deleting %s", path);
  else
    PASS("Deleting %s", path);

  /* Test directory creation */
  snprintf(buf, 128, "file:///%scibyl", fs_root);
  r = mkdir(buf, 0777);
  if (r < 0)
    FAIL("mkdir: %d", r);
  else
    PASS("mkdir: %d", r);

  /* A file in the directory */
  snprintf(buf, 128, "file:///%scibyl/a", fs_root);
  fp = NOPH_FileConnection_openFILE(buf, "w");
  if (!fp)
    FAIL("mkdir/file %s", buf);
  else
    {
      PASS("mkdir/file %s", buf);
      r = fputs("Maboo", fp);
      if (r < 0)
        FAIL("mkdir/file/fputs %d", r);
      else
        PASS("mkdir/file/fputs %d", r);

      fclose(fp);
      r = remove(buf);
      if (r < 0)
        FAIL("mkdir/file/remove %d", r);
      else
        PASS("mkdir/file/remove %d", r);
    }

  snprintf(buf, 128, "file:///%scibyl", fs_root);

  /* Delete the directory */
  r = rmdir(buf);
  if (r < 0)
    FAIL("rmdir: %d", r);
  else
    PASS("rmdir: %d", r);
}
#endif
