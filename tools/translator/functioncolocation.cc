/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      functioncolocation.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Method coloc
 *
 * $Id:$
 *
 ********************************************************************/
#include <functioncolocation.hh>
#include <utils.h>

FunctionColocation::FunctionColocation(const char *str)
{
  char *cpy = xstrdup(str);
  char *p = cpy;

  this->in_str = xstrdup(str);
  this->fns = NULL;
  this->fn_names = NULL;
  this->n_fns = 0;

  while (*p)
    {
      int n = this->n_fns;

      this->n_fns++;
      this->fns = (Function**)xrealloc(this->fns, sizeof(Function*) * this->n_fns);
      this->fn_names = (const char**)xrealloc(this->fn_names, sizeof(const char*) * this->n_fns);

      char *pnext = strchr(p, ';');
      if (pnext)
        {
          *pnext = '\0';
        }
      this->fn_names[n] = xstrdup(p);
      FunctionColocation::name_to_coloc[this->fn_names[n]] = this;

      this->fns[n] = NULL;

      if (pnext)
        {
          p = pnext+1;
        }
      else
        {
          p += strlen(p);
        }
    }
  panic_if(this->n_fns == 0,
           "Function coloc string %s has the wrong format\n", str);
}

void FunctionColocation::addFunction(Function *fn)
{
  const char *name = fn->getRealName();

  for (int i = 0; i < this->n_fns; i++)
    {
      if (strcmp(name, this->fn_names[i]) == 0)
        {
          this->fns[i] = fn;
          return;
        }
    }
  panic("Function %s not found in coloc\n", name);
}

JavaMethod *FunctionColocation::createJavaMethod()
{
  for (int i = 0; i < this->n_fns; i++)
    panic_if(!this->fns[i], "Colocating %s failed: %s not found\n",
        this->in_str, this->fn_names[i]);

  return new JavaMethod(this->fns, 0, this->n_fns-1);
}

FunctionColocation *FunctionColocation::lookup(const char *fn_name)
{
  return FunctionColocation::name_to_coloc[fn_name];
}

map<const char *, FunctionColocation *, cmp_str>FunctionColocation::name_to_coloc;
