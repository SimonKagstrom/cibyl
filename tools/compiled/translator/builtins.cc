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

BuiltinFactory::BuiltinFactory()
{
}

Builtin* BuiltinFactory::match(const char *name)
{
  return NULL;
}
