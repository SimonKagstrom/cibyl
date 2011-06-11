/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      syscall-wrappers.hh
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Syscall wrapper generator stuff
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __SYSCALL_WRAPPERS_HH__
#define __SYSCALL_WRAPPERS_HH__

#include <syscall.hh>
#include <controller.hh>

class SyscallWrapperGenerator
{
public:
  /**
   * Create a new syscall wrapper generator.
   *
   * @param defines null-terminated list of defines
   * @param dstdir the destination directory
   * @param n_syscall_dir the number of syscall directory names
   * @param syscall_dirs a vector of syscall directory names (absolute paths)
   * @param n_syscall_sets the number of syscall set names
   * @param syscall_sets a vector of syscall directory set names
   * @param used_syscalls a table of syscall_db_entry_t's mapped by
   * syscall names. This should be freed by the calling entity
   */
  SyscallWrapperGenerator(const char **defines, const char *dstdir,
                          int n_syscall_dirs, char **syscall_dirs,
                          int n_syscall_sets, char **syscall_sets,
                          Controller::CibylDbTable_t &used_syscalls);

  /* No pass1 - this will not be created until after pass1 */
  bool pass2();

private:
  const char *getJavaReturnString(int r);

  const char *lookupFilePath(const char *filename);

  void doOneArgumentGet(cibyl_db_entry_t *p, cibyl_db_arg_t *a );

  void doOne(cibyl_db_entry_t *p);

  void generateImports();

  void generateInits();

  void generateHelperClasses();

  void doOneNonGenerated(const char *dirname,
                         cibyl_db_entry_t *p);

  const char **defines;
  const char *m_dstdir;
  int n_syscall_dirs;
  char **syscall_dirs;
  char *m_globalSyscallDirectory;
  int n_syscall_sets;
  char **syscall_sets;
  int *set_usage;
  Controller::CibylDbTable_t &m_used_syscalls;
};

#endif /* !__SYSCALL_WRAPPERS_HH__ */
