/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      javaclass.hh
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Java class def
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __JAVACLASS_HH__
#define __JAVACLASS_HH__

#include <javamethod.hh>
#include <instruction.hh>

class JavaClass : public CodeBlock
{
public:
  JavaClass(const char *name, JavaMethod **methods, int first, int last);

  virtual JavaMethod *getMethodByAddress(uint32_t addr, int *idx);

  virtual JavaMethod *getMethodByAddress(uint32_t addr);

  int getNumberOfMethods()
  {
    return this->n_methods;
  }

  JavaMethod *getMethodByIndex(int idx)
  {
    if (idx < 0 || idx > this->n_methods)
      return NULL;

    return this->methods[idx];
  }

  virtual bool pass1();

  virtual bool pass2();

  const char *getName();

  const char *getFileName()
  {
    return this->filename;
  }

protected:
  int n_methods;
  int n_multiFunctionMethods; /* Will be placed last in the vector */
  JavaMethod **methods;
  const char *name;
  char *filename;
};

class CallTableClass : public JavaClass
{
public:
  CallTableClass(const char *name, JavaMethod **methods, int first, int last);

  virtual bool pass1();

  virtual bool pass2();

  virtual JavaMethod *getMethodByAddress(uint32_t addr);

  virtual JavaMethod *getMethodByAddress(uint32_t addr, int *idx);
};

#endif /* !__JAVACLASS_HH__ */
