/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      javaclass.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Implementation of java classes
 *
 * $Id:$
 *
 ********************************************************************/
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <javaclass.hh>
#include <emit.hh>

/* Some utilities */
static int method_cmp(const void *_a, const void *_b)
{
  JavaMethod *a = *(JavaMethod**)_a;
  JavaMethod *b = *(JavaMethod**)_b;

  return a->getAddress() - b->getAddress();
}

static int method_search_cmp(const void *_a, const void *_b)
{
  uint32_t *key = (uint32_t*)_a;
  JavaMethod *b = *(JavaMethod**)_b;

  if ( *key >= b->getAddress() && *key <= b->getAddress() + b->getSize() )
    return 0;

  if ( *key < b->getAddress() )
    return -1;

  /* After */
  return 1;
}

/* Class members */
JavaClass::JavaClass(const char *name, JavaMethod **in_methods, int first, int last) : CodeBlock()
{
  panic_if(last < first,
           "Creating a class with methods from %d to %d\n", first, last);

  this->methods = &(in_methods[first]);
  this->n_methods = last - first + 1;

  this->name = xstrdup(name);
  this->filename = (char*)xcalloc(strlen(name) + 8, sizeof(char));
  xsnprintf(this->filename, strlen(name) + 8, "%s.j", this->name);

  /* Sort the methods */
  qsort((void*)this->methods, this->n_methods, sizeof(JavaMethod*),
	method_cmp);
}

JavaMethod *JavaClass::getMethodByAddress(uint32_t addr, int *idx)
{
  uint32_t tmp = addr;
  JavaMethod **ret;

  *idx = -1; /* Assume not found */
  ret = (JavaMethod**)bsearch(&tmp, this->methods, this->n_methods,
                              sizeof(JavaMethod*), method_search_cmp);

  if (ret == NULL)
    return NULL;

  *idx = ret - this->methods;
  return *ret;
}

JavaMethod *JavaClass::getMethodByAddress(uint32_t addr)
{
  int dummy;

  return this->getMethodByAddress(addr, &dummy);
}

const char *JavaClass::getName()
{
  return this->name;
}


bool JavaClass::pass1()
{
  bool out = true;

  for (int i = 0; i < this->n_methods; i++)
    {
      if (this->methods[i]->pass1() != true)
	out = false;

      this->methods[i]->sortJumptabLabels();
    }
  return out;
}

bool JavaClass::pass2()
{
  bool out = true;

  emit->generic(".class public %s\n"
                ".super java/lang/Object\n\n"
                ".method public <init>()V\n"
                "\taload_0\n"
                "\tinvokenonvirtual java/lang/Object.<init>()V\n"
                "\treturn\n"
                ".end method\n",
                this->getName());

  for (int i = 0; i < this->n_methods; i++)
    {
      if (this->methods[i]->pass2() != true)
	out = false;
    }
  return out;
}


CallTableClass::CallTableClass(const char *name, JavaMethod **methods, int first, int last) : JavaClass(name, methods, first, last)
{
  free(this->filename); /* Alloced by the base constructor */

  this->filename = (char*)xcalloc(strlen(name) + 8, sizeof(char));
  xsnprintf(this->filename, strlen(name) + 8, "%s.java", this->name);
}


bool CallTableClass::pass1()
{
  return this->methods[0]->pass1();
}

bool CallTableClass::pass2()
{
  bool out = true;

  emit->generic("class CibylCallTable {\n");

  out = this->methods[0]->pass2();

  emit->generic("}\n");

  return out;
}

JavaMethod *CallTableClass::getMethodByAddress(uint32_t addr)
{
  return NULL;
}

JavaMethod *CallTableClass::getMethodByAddress(uint32_t addr, int *idx)
{
  return NULL;
}
