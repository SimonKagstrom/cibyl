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

  if ( *key >= b->getAddress() && *key < b->getAddress() + b->getSize() )
    return 0;

  if ( *key < b->getAddress() )
    return -1;

  /* After */
  return 1;
}

/* Class members */
JavaClass::JavaClass(JavaMethod **in_methods, int first, int last) : CodeBlock()
{
  assert( last >= first );

  this->methods = &(in_methods[first]);
  this->n_methods = last - first + 1;

  /* Sort the methods */
  qsort((void*)this->methods, this->n_methods, sizeof(JavaMethod*),
	method_cmp);
}

JavaMethod *JavaClass::getMethodByAddress(uint32_t addr)
{
  uint32_t tmp = addr;
  JavaMethod **ret;

  ret = (JavaMethod**)bsearch(&tmp, this->methods, this->n_methods,
                              sizeof(JavaMethod*), method_search_cmp);

  if (ret == NULL)
    {
      emit->error("Could not find method with address 0x%x\n",
                  addr);
      exit(1);
    }

  return *ret;
}

const char *JavaClass::getName()
{
  return "Cibyl";
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

  emit->bc_generic(".class public %s\n"
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
