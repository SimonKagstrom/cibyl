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
#include <config.hh>

#include "builtins/exceptions.cc"
#include "builtins/softfloat.cc"
#include "builtins/64-bit-muldiv.cc"

Instruction *tryInstruction;

Builtin::~Builtin()
{
}

BuiltinFactory::BuiltinFactory()
{
}

static bool cmp(const char *name, const char *key)
{
  return (strncmp(name, key, strlen(key)) == 0);
}

Builtin* BuiltinFactory::match(Instruction *insn, const char *name)
{
  /* Only look at the first part of the name */
  if (cmp(name, "__NOPH_try"))
    return new ExceptionBuiltinTry();
  if (cmp(name, "__NOPH_throw"))
    return new ThrowBuiltin();
  else if (cmp(name, "__NOPH_setjmp"))
    return new SetjmpBuiltin(name);

  /* Soft float optimization, the conversion can always be done */
  else if (cmp(name, "__floatsisf"))
    return new IntToFloat(name);
  else if (cmp(name, "__fixsfsi"))
    return new FloatToInt(name);
  else if (cmp(name, "__fixsfdi"))
    return new FloatToInt(name);

  /* 64-bit division */
  else if (cmp(name, "__divdi3"))
    return new DivBuiltin(name);
  else if (cmp(name, "__moddi3"))
    return new ModBuiltin(name);
  else if (cmp(name, "__ashrdi3"))
    return new ShrBuiltin(name);
  else if (cmp(name, "__ashldi3"))
    return new ShlBuiltin(name);

  if (config->optimizeInlines)
    {
      JavaMethod *mt = controller->getMethodByAddress(insn->getAddress());

      /* Some arbitrary value size limit! */
      if (mt->getBytecodeSize() < 20000)
        {
          if (cmp(name, "__negsf2"))
            return new Arithmetic1(name, "fneg");
          else if (cmp(name, "__addsf3"))
            return new Arithmetic2(name, "fadd");
          else if (cmp(name, "__subsf3"))
            return new Arithmetic2(name, "fsub");
          else if (cmp(name, "__divsf3"))
            return new Arithmetic2(name, "fdiv");
          else if (cmp(name, "__mulsf3"))
            return new Arithmetic2(name, "fmul");
          /* Comparisons */
          else if (cmp(name, "__eqsf2"))
            return new Compare(name, "fcmpg");
          else if (cmp(name, "__nesf2"))
            return new Compare(name, "fcmpg");
          else if (cmp(name, "__lesf2"))
            return new Compare(name, "fcmpg");
          else if (cmp(name, "__ltsf2"))
            return new Compare(name, "fcmpl");
        }
    }

  return NULL;
}
