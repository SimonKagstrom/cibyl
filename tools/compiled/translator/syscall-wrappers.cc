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
#include <sys/types.h>
#include <sys/stat.h>


SyscallWrapperGenerator::SyscallWrapperGenerator(int n_syscall_dirs, char **syscall_dirs,
                                                 ght_hash_table_t *used_syscalls)
{
  this->n_syscall_dirs = n_syscall_dirs;
  this->syscall_dirs = syscall_dirs;
  this->used_syscalls = used_syscalls;
}

const char *SyscallWrapperGenerator::getJavaReturnString(int r)
{
  static const char *table[] = {
    "void",
    "int",
    "boolean",
    "int" /* Object refrence */
  };

  if (r < 0 || r > 3)
    {
      fprintf(stderr,
              "Error getting Java return argument, %d is out of bounds\n",
              r);
      exit(1);
    }

  return table[r];
}

void SyscallWrapperGenerator::doOneArgumentGet(cibyl_db_entry_t *p, cibyl_db_arg_t *a )
{
  if ( strcmp(a->javaType, "void") == 0 || strcmp(a->javaType, a->type) == 0)
    return;
  emit->bc_generic("    %s %s = ", a->javaType, a->name);
  if ( strcmp(a->javaType, "boolean") == 0 )
    emit->bc_generic("(boolean) (__%s == 0 ? false : true)",
                     a->name);
  else if ( strcmp(a->javaType, "String") )
    emit->bc_generic("CRunTime.charPtrToString(__%s)",
                     a->name);
  else
    emit->bc_generic("(%s)CRunTime.objectRepository[__%s]",
                     a->javaType,
                     a->name, a->name);
  emit->bc_generic(";\n");
}

void SyscallWrapperGenerator::doOneNonGenerated(const char *dir,
                                                cibyl_db_entry_t *p)
{
  char buf[2048];
  const char* data;
  size_t size;
  int n;

  n = snprintf(buf, 2048, "%s/%s/implementation/%s.java",
               dir, p->set,  p->name);
  if (n >= 2048 || n < 1)
    {
      fprintf(stderr, "snprintf failed: %d\n", n);
      exit(1);
    }

  data = (const char*)read_file(buf, &size);
  if (!data)
    {
      fprintf(stderr, "Reading %s failed\n", buf);
      exit(1);
    }
  emit->bc_generic("%s", data);

  free((void*)data);
}

void SyscallWrapperGenerator::doOne(cibyl_db_entry_t *p)
{
  unsigned int n = 0;

  emit->bc_generic("  public static final %s %s(",
                   p->returns ? "int" : "void", p->name);
  for (unsigned int i = 0; i < p->nrArgs; i++)
    {
      cibyl_db_arg_t *a = &p->args[i];

      emit->bc_generic("int %s%s%s",
                       /* C and Java type the same? */
                       strcmp(a->type, a->javaType) == 0 ? "" : "__",
                       a->name,
                       i == p->nrArgs-1 ? "" : ", ");
    }
  emit->bc_generic(") %s {\n",
                   (p->qualifier & CIBYL_DB_QUALIFIER_THROWS) != 0 ? "throws Exception" : "");
  for (unsigned int i = 0; i < p->nrArgs; i++)
    this->doOneArgumentGet(p, &p->args[i]);

  if (p->returns)
    emit->bc_generic("    %s ret = (%s)",
                     this->getJavaReturnString(p->returns),
                     this->getJavaReturnString(p->returns));
  else
    emit->bc_generic("    ");

  /* Generate the call */
  if ( strcmp(p->javaMethod, "new") == 0 ) /* Constructor */
    emit->bc_generic("new %s(", p->javaClass);
  else
    {
      if (p->nrArgs != 0 &&
          (p->args[0].flags & CIBYL_DB_ARG_OBJREF))
        {
          /* Object call */
          emit->bc_generic("%s.%s(", p->args[0].name,
                           p->javaMethod);
          n++; /* Skip the first argument */
        }
      else /* Static call */
        emit->bc_generic("%s.%s(", p->javaClass,
                         p->javaMethod);
    }

  /* Pass all arguments */
  for ( ; n < p->nrArgs; n++ )
    emit->bc_generic("%s%s", p->args[n].name,
                     n == p->nrArgs-1 ? "" : ", ");
  emit->bc_generic(");\n");

  if ( p->returns )
    {
      const char *rn = "ret";

      if (p->returns == CIBYL_DB_RETURN_OBJREF)
        {
          emit->bc_generic("    int registeredHandle = CRunTime.registerObject(%s);",
                           rn);
          rn = "registeredHandle";
        }

      if (p->returns == CIBYL_DB_RETURN_BOOLEAN)
        emit->bc_generic("    return %s ? 1 : 0;\n", rn);
      else
        emit->bc_generic("    return %s;\n", rn);
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
      if ((p->qualifier & CIBYL_DB_QUALIFIER_NOT_GENERATED) == 0)
        this->doOne(p);
      else
        this->doOneNonGenerated(this->syscall_dirs[p->user],
                                p);
    }
  emit->bc_generic("}\n");

  return true;
}
