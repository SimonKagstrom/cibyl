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


SyscallWrapperGenerator::SyscallWrapperGenerator(const char **defines, const char *dstdir,
                                                 int n_syscall_dirs, char **syscall_dirs,
                                                 int n_syscall_sets, char **syscall_sets,
                                                 ght_hash_table_t *used_syscalls)
{
  cibyl_db_entry_t *p;
  ght_iterator_t it;
  const void *key;

  this->dstdir = dstdir;
  this->n_syscall_dirs = n_syscall_dirs;
  this->syscall_dirs = syscall_dirs;
  this->n_syscall_sets = n_syscall_sets;
  this->syscall_sets = syscall_sets;
  this->used_syscalls = used_syscalls;
  this->defines = defines;

  this->set_usage = (int*)xcalloc( n_syscall_sets, sizeof(int) );

  /* Fixup set usage */
  for (p = (cibyl_db_entry_t *)ght_first(this->used_syscalls, &it, &key);
       p;
       p = (cibyl_db_entry_t *)ght_next(this->used_syscalls, &it, &key))
    {
      /* Typically ~10 syscall sets, so this should be OK */
      for (int i = 0; i < this->n_syscall_sets; i++)
        {
          if ( strcmp(p->set, this->syscall_sets[i]) == 0 )
            this->set_usage[i]++;
        }
    }
}

void SyscallWrapperGenerator::doOneArgumentGet(cibyl_db_entry_t *p, cibyl_db_arg_t *a )
{
  if ( strcmp(a->javaType, "void") == 0 || strcmp(a->javaType, a->type) == 0)
    return;
  emit->generic("    %s %s = ", a->javaType, a->name);
  if ( strcmp(a->javaType, "boolean") == 0 )
    emit->generic("(boolean) (__%s == 0 ? false : true)",
                  a->name);
  else if ( strcmp(a->javaType, "String") == 0 )
    emit->generic("CRunTime.charPtrToString(__%s)",
                  a->name);
  else
    emit->generic("(%s)CRunTime.objectRepository[__%s]",
                  a->javaType,
                  a->name, a->name);
  emit->generic(";\n");
}

void SyscallWrapperGenerator::doOneNonGenerated(const char *dir,
                                                cibyl_db_entry_t *p)
{
  const char* data;
  size_t size;

  data = (const char*)read_cpp(&size, this->defines, "%s/%s/implementation/%s.java",
                               dir, p->set, p->name);
  if (!data)
    {
      fprintf(stderr, "Reading %s:%s failed\n", p->set, p->name);
      exit(1);
    }
  emit->output(data);

  free((void*)data);
}

void SyscallWrapperGenerator::doOne(cibyl_db_entry_t *p)
{
  unsigned int n = 0;

  emit->generic("  public static final %s %s(",
                p->returns ? "int" : "void", p->name);
  for (unsigned int i = 0; i < p->nrArgs; i++)
    {
      cibyl_db_arg_t *a = &p->args[i];

      emit->generic("int %s%s%s",
                    /* C and Java type the same? */
                    strcmp(a->type, a->javaType) == 0 ? "" : "__",
                    a->name,
                    i == p->nrArgs-1 ? "" : ", ");
    }
  emit->generic(") %s {\n",
                (p->qualifier & CIBYL_DB_QUALIFIER_THROWS) != 0 ? "throws Exception" : "");
  for (unsigned int i = 0; i < p->nrArgs; i++)
    this->doOneArgumentGet(p, &p->args[i]);

  if (p->returns)
    emit->generic("    %s ret = (%s)",
                  p->returnType,
                  p->returnType);
  else
    emit->generic("    ");

  /* Generate the call */
  if ( strcmp(p->javaMethod, "new") == 0 ) /* Constructor */
    emit->generic("new %s(", p->javaClass);
  else
    {
      if (p->nrArgs != 0 &&
          (p->args[0].flags & CIBYL_DB_ARG_OBJREF) &&
          (strcmp(p->javaClass, p->args[0].javaType) == 0))
        {
          /* Object call */
          emit->generic("%s.%s(", p->args[0].name,
                        p->javaMethod);
          n++; /* Skip the first argument */
        }
      else /* Static call */
        emit->generic("%s.%s(", p->javaClass,
                      p->javaMethod);
    }

  /* Pass all arguments */
  for ( ; n < p->nrArgs; n++ )
    emit->generic("%s%s", p->args[n].name,
                  n == p->nrArgs-1 ? "" : ", ");
  emit->generic(");\n");

  if ( p->returns )
    {
      const char *rn = "ret";

      if (p->returns == CIBYL_DB_RETURN_OBJREF)
        {
          emit->generic("    int registeredHandle = CRunTime.registerObject(%s);\n",
                        rn);
          rn = "registeredHandle";
        }

      if (p->returns == CIBYL_DB_RETURN_BOOLEAN)
        emit->generic("    return %s ? 1 : 0;\n", rn);
      else
        emit->generic("    return %s;\n", rn);
    }

  emit->generic("  }\n");
}

void SyscallWrapperGenerator::generateImports()
{
  /* The syscall sets do not carry dir information, thus loop through
   * all dirs and all sets */
  for (int i = 0; i < this->n_syscall_dirs; i++)
    {
      char *dir = this->syscall_dirs[i];

      for (int j = 0; j < this->n_syscall_sets; j++)
        {
          char *cur = this->syscall_sets[j];
          const char* data;
          size_t size;

          if (this->set_usage[j] == 0)
            continue;

          data = (const char*)read_cpp(&size, this->defines, "%s/%s/imports",
                                       dir, cur);
          if (data)
            emit->generic("%s", data);
        }
    }
}

void SyscallWrapperGenerator::generateInits()
{
  /* The syscall sets do not carry dir information, thus loop through
   * all dirs and all sets */
  for (int i = 0; i < this->n_syscall_dirs; i++)
    {
      char *dir = this->syscall_dirs[i];

      for (int j = 0; j < this->n_syscall_sets; j++)
        {
          char *cur = this->syscall_sets[j];
          const char* data;
          size_t size;

          if (this->set_usage[j] == 0)
            continue;

          data = (const char*)read_cpp(&size, this->defines, "%s/%s/init",
                                       dir, cur);
          if (data)
            emit->output(data);
        }
    }
}


void SyscallWrapperGenerator::generateHelperClasses()
{
  /* The syscall sets do not carry dir information, thus loop through
   * all dirs and all sets */
  for (int i = 0; i < this->n_syscall_dirs; i++)
    {
      char *dirname = this->syscall_dirs[i];

      for (int j = 0; j < this->n_syscall_sets; j++)
        {
          char *cur = this->syscall_sets[j];
          struct dirent *de;
          DIR *dir;

          if (this->set_usage[j] == 0)
            continue;

          dir = open_dir_fmt("%s/%s/classes", dirname, cur);

          if (!dir)
            continue;
          de = readdir(dir);
          while(de)
            {
              const char* data;
              size_t size;

              /* Only match *.java */
              if ( !strstr(de->d_name, ".java") )
                {
                  de = readdir(dir);
                  continue;
                }

              data = (const char*)read_cpp(&size, this->defines, "%s/%s/classes/%s",
                                           dirname, cur, de->d_name);
              if (data)
                {
                  emit->setOutputFile(open_file_in_dir(this->dstdir, de->d_name, "w"));
                  emit->output(data);
                }
              de = readdir(dir);
            }
        }
    }
}

bool SyscallWrapperGenerator::pass2()
{
  cibyl_db_entry_t *p;
  ght_iterator_t it;
  const void *key;

  emit->setOutputFile(open_file_in_dir(this->dstdir, "Syscalls.java", "w"));
  emit->generic("/* GENERATED, DON'T EDIT */\n");
  this->generateImports();
  emit->generic("public class Syscalls {\n");

  this->generateInits();

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
  emit->generic("}\n");
  this->generateHelperClasses();

  return true;
}
