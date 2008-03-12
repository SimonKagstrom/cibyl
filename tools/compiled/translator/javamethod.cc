/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      javamethod.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Java method implementation
 *
 * $Id:$
 *
 ********************************************************************/
#include <stdio.h>
#include <string.h>
#include <javamethod.hh>
#include <controller.hh>
#include <emit.hh>

JavaMethod::JavaMethod(Function **fns,
		       int first, int last) : CodeBlock()
{
  if (fns)
    {
      this->functions = &(fns[first]);
      this->n_functions = last - first + 1;
    }
  else
    {
      this->functions = NULL;
      this->n_functions = 0;
    }

  this->javaName = NULL;
  this->n_registersToPass = 0;
  this->registerIndirectJumps = false;

  this->address = 0;
  this->size = 0;
  for (int i = 0; i < this->n_functions; i++)
    {
      /* The address is the lowest of the functions' */
      if ( this->functions[i]->getAddress() > this->address)
	this->address = this->functions[i]->getAddress();
      this->size += this->functions[i]->getSize();
    }

  memset(this->registerUsage, 0, sizeof(this->registerUsage));
  this->n_exceptionHandlers = 0;
  this->exceptionHandlers = NULL;
}

JavaMethod::~JavaMethod()
{
}

char *JavaMethod::getJavaMethodName()
{
  if ( this->javaName )
    return this->javaName;
  int n = this->n_registersToPass;

  /* THIS is soo much nicer in Python! */
  int len = strlen(this->functions[0]->getName()) + n + 8;
  this->javaName = (char*)xcalloc( len, sizeof(char));

  int o = snprintf(this->javaName, len, "%s(", this->getName());
  for (; n > 0; n--, o++)
    this->javaName[o] = 'I';
  this->javaName[o++] = ')';

  if ( this->clobbersReg(R_V0) )
    this->javaName[o++] = 'I';
  else
    this->javaName[o++] = 'V';

  return this->javaName;
}

bool JavaMethod::pass1()
{
  bool out = true;
  void *it;

  for (int i = 0; i < this->n_functions; i++)
    {
      Function *fn = this->functions[i];

      if ( !fn->pass1() )
	out = false;
      if ( fn->hasRegisterIndirectJumps() )
        this->registerIndirectJumps = true;
    }

  for (int i = 0; i < this->n_functions; i++)
    {
      Function *fn = this->functions[i];

      fn->fillDestinations(this->registerUsage);
      fn->fillSources(this->registerUsage);
    }

  for (MIPS_register_t reg = this->getFirstRegisterToPass(&it); reg != R_ZERO; reg = this->getNextRegisterToPass(&it))
    this->n_registersToPass++;

  return out;
}

bool JavaMethod::pass2()
{
  bool out = true;

  regalloc->setAllocation(this->registerUsage);

  emit->generic("\n.method public static %s\n"
                ".limit stack 20\n"
                ".limit locals %d\n",
                this->getJavaMethodName(),
                regalloc->getNumberOfLocals());

  /* Emit register mapping */
  for (int i = 0; i < N_REGS; i++)
    {
      if (this->registerUsage[i])
        emit->generic("; local %2d is register %s\n",
                      regalloc->regToLocal((MIPS_register_t)i),
                      mips_reg_strings[i]);
    }

  /* Zero all used registers */
  for (int i = 0; i < N_REGS; i++)
    {
      /* These are passed as arguments, skip them */
      if (i == R_SP || i == R_A0 || i == R_A1 || i == R_A2 || i == R_A3)
        continue;

      if (this->registerUsage[i] > 0)
        {
          /* This register is used, zero */
          emit->bc_pushconst(0);
          emit->bc_popregister((MIPS_register_t)i);
        }
    }

  for (int i = 0; i < this->n_functions; i++)
    {
      Function *fn = this->functions[i];

      /* And compile the function */
      if ( !fn->pass2() )
	out = false;
    }

  if (this->registerIndirectJumps)
    {
      emit->bc_goto("__CIBYL_function_return");
      emit->bc_label("__CIBYL_local_jumptab");

      /* Emit the actual jumptab */
      emit->bc_lookupswitch(this->n_jumptabLabels, this->jumptabLabels,
                            "__CIBYL_function_return");
    }

  for (int i = 0; i < this->n_exceptionHandlers; i++)
    {
      ExceptionHandler *eh = this->exceptionHandlers[i];

      emit->bc_label("%s", eh->name);
      /* Register the object passed here */
      emit->bc_invokestatic("CRunTime/registerObject(Ljava/lang/Object;)I");
      emit->bc_pushregister(R_ECB);
      emit->bc_swap();
      /* This is just a jalr(ecb, sp, ear (exception obj)) */
      emit->bc_pushregister(R_SP);
      emit->bc_swap();
      emit->bc_pushregister(R_EAR);
      emit->bc_pushconst(0);
      emit->bc_pushconst(0);
      emit->bc_invokestatic(controller->getCallTableMethod()->getJavaMethodName());
      emit->bc_pop();
      emit->bc_goto( eh->end );
    }

  emit->bc_label("__CIBYL_function_return");
  if (this->clobbersReg(R_V1))
    {
      emit->bc_pushregister(R_V1);
      emit->bc_putstatic("CRunTime/saved_v1 I");
    }
  if (this->clobbersReg(R_V0))
    {
      emit->bc_pushregister(R_V0);
      emit->bc_ireturn();
    }
  else
    emit->bc_return();

  emit->generic(".end method ; %s\n", this->getJavaMethodName());

  return out;
}

/* R_ZERO terminates it */
static MIPS_register_t possibleArgumentRegs[] = { R_SP, R_A0, R_A1, R_A2, R_A3, R_ZERO };

MIPS_register_t JavaMethod::getFirstRegisterToPass(void *_it)
{
  int *it = (int*)_it;
  MIPS_register_t ret;

  *it = 0;

  while (possibleArgumentRegs[*it] != R_ZERO &&
	 this->clobbersReg( possibleArgumentRegs[*it] ) == 0)
      (*it)++;
  ret = possibleArgumentRegs[*it];
  (*it)++;

  return ret;
}

MIPS_register_t JavaMethod::getNextRegisterToPass(void *_it)
{
  int *it = (int*)_it;
  MIPS_register_t ret;

  while (possibleArgumentRegs[*it] != R_ZERO &&
	 this->clobbersReg( possibleArgumentRegs[*it] ) == 0)
      (*it)++;
  ret = possibleArgumentRegs[*it];
  (*it)++;
  return ret;
}

bool JavaMethod::clobbersReg(MIPS_register_t reg)
{
  return this->registerUsage[reg] > 0;
}

char *JavaMethod::addExceptionHandler(uint32_t start, uint32_t end)
{
  int n = this->n_exceptionHandlers;
  ExceptionHandler *eh;

  this->exceptionHandlers = (ExceptionHandler**)xrealloc(this->exceptionHandlers,
                                                         (n + 1) * sizeof(ExceptionHandler*) );
  this->n_exceptionHandlers = n + 1;

  eh = new ExceptionHandler(start, end);
  this->exceptionHandlers[n] = eh;

  return eh->name;
}
