/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      softfloat.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Softfloat builtins implementation
 *
 * $Id:$
 *
 ********************************************************************/
#include <javamethod.hh>
#include <controller.hh>
#include <builtins.hh>
#include <emit.hh>

class Arithmetic1 : public Builtin
{
public:
  Arithmetic1(const char *name, const char *bc) : Builtin(name)
  {
    this->bc = bc;
  }

  bool pass1(Instruction *insn)
  {
    return true;
  }

  bool pass2(Instruction *insn)
  {
    /* Convert the source register */
    emit->bc_pushregister(R_A0);
    emit->bc_invokestatic("java/lang/Float/intBitsToFloat(I)F");

    /* The actual operation */
    emit->bc_generic_insn(this->bc);

    /* Pop the result to v0 */
    emit->bc_invokestatic("java/lang/Float/floatToIntBits(F)I");
    emit->bc_popregister(R_V0);
    return true;
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(R_A0, p);
  };

  int fillDestinations(int *p)
  {
    return this->addToRegisterUsage(R_V0, p);
  };
private:
  const char *bc;
};


class Arithmetic2 : public Builtin
{
public:
  Arithmetic2(const char *name, const char *bc) : Builtin(name)
  {
    this->bc = bc;
  }

  bool pass1(Instruction *insn)
  {
    return true;
  }

  bool pass2(Instruction *insn)
  {
    /* Convert the source registers */
    emit->bc_pushregister(R_A0);
    emit->bc_invokestatic("java/lang/Float/intBitsToFloat(I)F");
    emit->bc_pushregister(R_A1);
    emit->bc_invokestatic("java/lang/Float/intBitsToFloat(I)F");

    /* The actual operation */
    emit->bc_generic_insn(this->bc);

    /* Pop the result to v0 */
    emit->bc_invokestatic("java/lang/Float/floatToIntBits(F)I");
    emit->bc_popregister(R_V0);
    return true;
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(R_A0, p) + this->addToRegisterUsage(R_A1, p);
  };

  int fillDestinations(int *p)
  {
    return this->addToRegisterUsage(R_V0, p);
  };
private:
  const char *bc;
};


class Compare : public Builtin
{
public:
  Compare(const char *name, const char *bc) : Builtin(name)
  {
    this->bc = bc;
  }

  bool pass1(Instruction *insn)
  {
    return true;
  }

  bool pass2(Instruction *insn)
  {
    /* Convert the source registers */
    emit->bc_pushregister(R_A0);
    emit->bc_invokestatic("java/lang/Float/intBitsToFloat(I)F");
    emit->bc_pushregister(R_A1);
    emit->bc_invokestatic("java/lang/Float/intBitsToFloat(I)F");

    /* The actual operation */
    emit->bc_generic_insn(this->bc);

    /* Pop the result to v0 */
    emit->bc_popregister(R_V0);
    return true;
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(R_A0, p) + this->addToRegisterUsage(R_A1, p);
  };

  int fillDestinations(int *p)
  {
    return this->addToRegisterUsage(R_V0, p);
  };
private:
  const char *bc;
};


class IntToFloat : public Builtin
{
public:
  IntToFloat(const char *name) : Builtin(name)
  {
  }

  bool pass1(Instruction *insn)
  {
    return true;
  }

  bool pass2(Instruction *insn)
  {
    /* Convert the source register */
    emit->bc_pushregister(R_A0);
    emit->bc_i2f();

    /* Pop the result to v0 */
    emit->bc_invokestatic("java/lang/Float/floatToIntBits(F)I");
    emit->bc_popregister(R_V0);
    return true;
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(R_A0, p);
  };

  int fillDestinations(int *p)
  {
    return this->addToRegisterUsage(R_V0, p);
  };
private:
  const char *bc;
};


class FloatToInt : public Builtin
{
public:
  FloatToInt(const char *name) : Builtin(name)
  {
  }

  bool pass1(Instruction *insn)
  {
    return true;
  }

  bool pass2(Instruction *insn)
  {
    /* Convert the source register */
    emit->bc_pushregister(R_A0);
    emit->bc_invokestatic("java/lang/Float/intBitsToFloat(I)F");
    emit->bc_f2i();

    /* Pop the result to v0 */
    emit->bc_popregister(R_V0);
    return true;
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(R_A0, p);
  };

  int fillDestinations(int *p)
  {
    return this->addToRegisterUsage(R_V0, p);
  };
private:
  const char *bc;
};
