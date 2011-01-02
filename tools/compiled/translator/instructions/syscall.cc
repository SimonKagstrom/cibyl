/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      syscall.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Calls to Java functionality
 *
 * $Id:$
 *
 ********************************************************************/
#include <controller.hh>

Syscall::Syscall(const char *name, int nrArguments, char returnValue)
{
  int len = strlen(name) + strlen(controller->getJasminPackagePath()) +
      nrArguments + 4 + strlen("Syscalls/");
  int i, n;

  this->javaSignature = (char*)xcalloc( len, sizeof(char) );
  n = snprintf(this->javaSignature, len, "%sSyscalls/%s(",
      controller->getJasminPackagePath(), name);
  for (i = 0; i < nrArguments; i++)
    this->javaSignature[n + i] = 'I';
  this->javaSignature[n + i] = ')';
  i++;
  this->javaSignature[n + i] = returnValue;

  this->returnValue = returnValue;
  this->nrArguments = nrArguments;
}

class SyscallInsn : public Instruction
{
public:
  SyscallInsn(uint32_t address, int32_t extra) : Instruction(address,
		  CIBYL_SYSCALL, R_ZERO, R_ZERO, R_ZERO, extra)
  {
    this->sysc = NULL;
  }

  bool pass1()
  {
    /* FIXME: Mark syscall used */
    this->sysc = controller->getSyscall(this->extra);
    return true;
  }

  bool pass2()
  {
    emit->bc_invokestatic( this->sysc->getJavaSignature() );

    if ( this->sysc->returnsValue() )
      emit->bc_popregister( R_V0 );
    return true;
  }

  int fillDestinations(int *p)
  {
    if ( this->sysc->returnsValue() )
      return this->addToRegisterUsage(R_V0, p);
    return 0;
  };

  size_t getMaxStackHeight()
  {
    return max(this->sysc->getRegistersToPass(), 1);
  }

protected:
  Syscall *sysc;
};



class SyscallRegisterArgument : public Instruction
{
public:
  SyscallRegisterArgument(uint32_t address, int32_t extra) : Instruction(address,
		  CIBYL_REGISTER_ARGUMENT, R_ZERO, R_ZERO, R_ZERO, extra)
  {
  }

  bool pass1()
  {
    return true;
  }

  bool pass2()
  {
    emit->bc_pushregister( (MIPS_register_t)this->extra );
    return true;
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage((MIPS_register_t)this->extra, p);
  };

  size_t getMaxStackHeight()
  {
    return 1;
  }
};
