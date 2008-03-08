/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      syscall-wrappers.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Implementation of syscall wrappers
 *
 * $Id:$
 *
 ********************************************************************/
#include <emit.hh>
#include <syscall-wrappers.hh>


SyscallWrapperGenerator::SyscallWrapperGenerator(int n_syscall_dirs, char **syscall_dirs,
                                                 ght_hash_table_t *used_syscalls)
{
  this->n_syscall_dirs = n_syscall_dirs;
  this->syscall_dirs = syscall_dirs;
  this->used_syscalls = used_syscalls;
}

void SyscallWrapperGenerator::doOne(cibyl_db_entry_t *p)
{
  emit->bc_generic("  public static final %s %s(",
                   p->returns ? "int" : "void", p->name);
  for (unsigned int i = 0; i < p->nrArgs; i++)
    {
      emit->bc_generic("int __%s%s",
                       p->args[i].name,
                       i == p->nrArgs-1 ? "" : ",");
    }
  emit->bc_generic(") {\n");
  for (unsigned int i = 0; i < p->nrArgs; i++)
    {
      emit->bc_generic("    %s %s = (%s)CRunTime.objectRepository[__%s];\n",
                       p->args[i].javaType, p->args[i].name,
                       p->args[i].javaType,
                       p->args[i].name, p->args[i].name);
    }

  emit->bc_generic("  }\n");
}

bool SyscallWrapperGenerator::pass2()
{
  cibyl_db_entry_t *p;
  ght_iterator_t it;
  const void *key;

  emit->bc_generic("/* GENERATED, DON'T EDIT */\n");
  emit->bc_generic("public class Syscalls {\n");

  for (p = (cibyl_db_entry_t *)ght_first(this->used_syscalls, &it, &key);
       p;
       p = (cibyl_db_entry_t *)ght_next(this->used_syscalls, &it, &key))
    {
      this->doOne(p);
    }
  emit->bc_generic("}\n");

  return true;
}
