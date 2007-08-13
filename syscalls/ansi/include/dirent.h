/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      dirent.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   dirents for Cibyl
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __DIRENT_H__
#define __DIRENT_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>
#include <java/util.h>
#include <javax/microedition/io.h>

struct dirent
{
  char d_name[256]; /* filename */
};

typedef struct
{
  NOPH_FileConnection_t fc;
  NOPH_Enumeration_t it;
} DIR;


extern DIR *opendir(const char *name);
extern int closedir(DIR *dir);
extern struct dirent *readdir(DIR *dir);
extern int readdir_r(DIR *dir, struct dirent *entry,
                     struct dirent **result);

#if defined(__cplusplus)
}
#endif
#endif /* !__DIRENT_H__ */
