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


SyscallWrapperGenerator::SyscallWrapperGenerator(ght_hash_table_t *used_syscalls)
{
  this->used_syscalls = used_syscalls;
}



bool SyscallWrapperGenerator::pass2()
{
  emit->bc_generic("/* GENERATED, DON'T EDIT */");
  emit->bc_generic("public class Syscalls {");

  return true;
}
