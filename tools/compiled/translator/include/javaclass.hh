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
  JavaClass(JavaMethod **methods, int first, int last);

  JavaMethod *getMethodByAddress(uint32_t addr);

  bool pass1();

  bool pass2();

  char *getName();

private:
  int n_methods;
  JavaMethod **methods;
};

#endif /* !__JAVACLASS_HH__ */
