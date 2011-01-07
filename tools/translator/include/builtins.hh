/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      builtins.hh
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Implementation of builtins
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __BUILTINS_HH__
#define __BUILTINS_HH__

#include <instruction.hh>

class Builtin : public Entity
{
public:
  Builtin(const char *name) : Entity()
  {
    this->name = name;
  }

  virtual ~Builtin();

  const char *getName()
  {
    return this->name;
  }

  virtual bool pass1(Instruction *base) = 0;

  virtual bool pass2(Instruction *base) = 0;

  virtual int fillDestinations(int *p)
  {
    return 0;
  };

  virtual int fillSources(int *p)
  {
    return 0;
  };

private:
  const char *name;
};

class BuiltinFactory
{
public:
  BuiltinFactory();

  /**
   * Return a builtin object if there is one
   */
  Builtin *match(Instruction *insn, const char *name);

private:
  Builtin *builtins;
};

#endif /* !__BUILTINS_HH__ */
