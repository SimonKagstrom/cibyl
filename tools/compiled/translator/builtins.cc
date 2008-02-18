/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      builtins.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Implementation of builtins
 *
 * $Id:$
 *
 ********************************************************************/
#include <builtins.hh>
#include "builtins/exceptions.hh"

Instruction *tryInstruction;

Builtin::~Builtin()
{
}

BuiltinFactory::BuiltinFactory()
{
}

Builtin* BuiltinFactory::match(const char *name)
{
  /* Only look at the first part of the name */
  if (strncmp(name, "__NOPH_try", strlen("__NOPH_try")) == 0)
    return new ExceptionBuiltinTry();
  else if (strncmp(name, "__NOPH_catch", strlen("__NOPH_catch")) == 0)
    return new ExceptionBuiltinCatch();
  return NULL;
}
