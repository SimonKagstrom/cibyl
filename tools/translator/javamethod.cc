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
#include <config.hh>
#include <emit.hh>

bool ExceptionHandler::pass2()
{
  JavaClass *dstClass;
  JavaMethod *dstMethod;

  dstMethod = controller->getCallTableMethod();
  panic_if(!dstMethod,
           "No call table method???\n");
  dstClass = controller->getClassByMethodName(dstMethod->getName());
  panic_if(!dstClass,
           "No class for the call table method???\n");

  emit->bc_label("%s", this->name);
  /* Register the object passed here */
  emit->bc_invokestatic("%sCRunTime/registerObject(Ljava/lang/Object;)I",
      controller->getJasminPackagePath());
  emit->bc_pushregister(R_ECB);
  emit->bc_swap();
  /* This is just a jalr(ecb, sp, ear (exception obj)) */
  emit->bc_pushregister(R_SP);
  emit->bc_swap();
  emit->bc_pushregister(R_EAR);
  emit->bc_pushconst(0);
  emit->bc_pushconst(0);
  emit->bc_invokestatic("%s/%s", dstClass->getName(),
                        dstMethod->getJavaMethodName());
  emit->bc_pop();
  emit->bc_goto( this->end );

  return true;
}

JavaMethod::JavaMethod(Function **fns,
		       int first, int last) : CodeBlock()
{
  panic_if(first > last,
           "First and last %d:%d in method constructor are invalid\n",
           first, last);

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
  this->m_returnSize = -1;

  this->m_possibleArguments = (MIPS_register_t*)xcalloc(7,
      sizeof(MIPS_register_t));
  this->m_possibleArguments[0] = R_SP;
  this->m_possibleArguments[1] = R_FNA;
  this->m_possibleArguments[2] = R_A0;
  this->m_possibleArguments[3] = R_A1;
  this->m_possibleArguments[4] = R_A2;
  this->m_possibleArguments[5] = R_A3;
  this->m_possibleArguments[6] = R_ZERO;

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

  this->n_returnLocations = 0;
  this->returnLocations = NULL;

  /* Fixup the bytecode size */
  this->bc_size = 0;
  this->maxStackHeight = 0;
  for (int i = 0; i < this->n_functions; i++)
    this->bc_size += this->functions[i]->getBytecodeSize();
}

JavaMethod::~JavaMethod()
{
  free(this->m_possibleArguments);
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

  if ( config->threadSafe )
    {
      if ( this->returnSize() == 2)
        this->javaName[o++] = 'J'; /* long */
      else if ( this->returnSize() == 1)
        this->javaName[o++] = 'I';
      else
        this->javaName[o++] = 'V';
    }
  else
    {
      if ( this->returnSize() >= 1)
        this->javaName[o++] = 'I';
      else
        this->javaName[o++] = 'V';
    }

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
      this->maxStackHeight = max(this->maxStackHeight, fn->getMaxStackHeight());
    }

  for (int i = 0; i < this->n_functions; i++)
    {
      Function *fn = this->functions[i];

      fn->fillDestinations(this->registerUsage);
      fn->fillSources(this->registerUsage);
    }

  /* If this is a multi-function method, it will also use R_FNA as
   * a method argument */
  if (this->hasMultipleFunctions())
    this->registerUsage[R_FNA]++;

  ElfSymbol *sym = elf->getSymbolByAddr(this->getAddress());
  /* First see which arguments need to be passed */
  if (config->optimizeFunctionReturnArguments && sym && sym->n_args >= 0)
    {
      /* Zero-terminate at the last needed reg */
      if (sym->n_args <= 3)
        this->m_possibleArguments[5] = R_ZERO;
      if (sym->n_args <= 2)
        this->m_possibleArguments[4] = R_ZERO;
      if (sym->n_args <= 1)
        this->m_possibleArguments[3] = R_ZERO;
      if (sym->n_args == 0)
        this->m_possibleArguments[2] = R_ZERO;
    }

  for (MIPS_register_t reg = this->getFirstRegisterToPass(&it);
      reg != R_ZERO;
      reg = this->getNextRegisterToPass(&it))
    this->n_registersToPass++;

  /* Setup the size of the return value - 0 for functions
   * that return nothing, 1 for 32-bit returns and 2 for 64-bit
   * dittos */
  this->setReturnSize(0);

  if (this->clobbersReg(R_V0) && this->clobbersReg(R_V1))
    this->setReturnSize(2);
  else if (this->clobbersReg(R_V0))
    this->setReturnSize(1);
  else if (this->clobbersReg(R_V1))
    emit->warning("Function clobbers V1 but not V0");

  /* Update the size from the ELF, if it's known */
  if (config->optimizeFunctionReturnArguments && sym)
    {
      if (sym->ret_size >= 0 && sym->ret_size <= 2)
        this->setReturnSize(sym->ret_size);
    }

  return out;
}


int JavaMethod::addReturnLocation(uint32_t address)
{
  int out = this->n_returnLocations;

  this->n_returnLocations++;
  this->returnLocations = (uint32_t*)xrealloc(this->returnLocations,
                                              this->n_returnLocations * sizeof(uint32_t*));

  this->returnLocations[out] = address;
  controller->getBranchTarget(address)->setBranchTarget();

  return out;
}

Function *JavaMethod::getFunctionByAddress(uint32_t addr)
{
  for (int i = 0; i < this->n_functions; i++)
    {
      Function *fn = this->functions[i];

      if ( addr >= fn->getAddress() && addr <= fn->getAddress() + fn->getSize() )
        return fn;
    }

  return NULL;
}

int JavaMethod::getFunctionIndexByAddress(uint32_t addr)
{
  for (int i = 0; i < this->n_functions; i++)
    {
      Function *fn = this->functions[i];

      if ( addr >= fn->getAddress() && addr <= fn->getAddress() + fn->getSize() )
        return i;
    }

  return -1;
}


void JavaMethod::emitStoreSubroutine(mips_opcode_t op)
{
}

void JavaMethod::emitLoadSubroutine(mips_opcode_t op)
{
  uint32_t and_value = 3; /* Assume byte*/
  uint32_t mask_value = 0xff;

  if (op == OP_LHU || op == OP_LH)
    {
      and_value = 2;
      mask_value = 0xffff;
    }

  /*
   * These are called with the following stack:
   *
   *          value to store (for sb/sh)
   *          address to load/store
   *   top -> return address
   */
  emit->bc_label("__CIBYL_memoryRead%s%s",
                 (op == OP_LBU || op == OP_LB) ? "Byte" : "Short",
                 (op == OP_LBU || op == OP_LHU) ? "Unsigned" : "");
  emit->bc_astore( R_MADR );

  /* The address to load is now on the top */
  emit->bc_dup();
  emit->bc_pushregister(R_MEM);
  emit->bc_swap();
  emit->bc_pushconst(2);
  emit->bc_iushr();      /* push addr >>> 2 */
  emit->bc_iaload();     /* and load the word */

  /* b = 3 - (address & 3) */
  emit->bc_swap();
  emit->bc_pushconst(and_value);
  emit->bc_iand();       /* push address & 3 */
  emit->bc_pushconst(and_value);
  emit->bc_swap();
  emit->bc_isub();       /* push 3 - (address & 3) */

  /* b = b * 8 */
  emit->bc_pushconst(3);
  emit->bc_ishl();

  /* Value now on the top, shift it down and mask it out */
  emit->bc_iushr();
  if (op == OP_LHU)
    emit->bc_i2c();
  else if (op == OP_LB)
    emit->bc_i2b(); /* Sign extension */
  else if (op == OP_LH)
    emit->bc_i2s(); /* Ditto */
  else
    {
      /* LBU */
      emit->bc_pushconst(mask_value);
      emit->bc_iand(); /* out = val >>> (b*8) */
    }

  /* Back again, now with the result on the top of the stack */
  emit->bc_ret( R_MADR );
}

void JavaMethod::emitSubroutineForOp(mips_opcode_t op)
{
  panic_if(op != OP_LB && op != OP_LBU && op != OP_LH &&
           op != OP_LHU && op != OP_SB && op != OP_SH,
           "Illegal opcode %s passed to emitLoadStoreSubroutines\n",
           mips_op_strings[op]);

  if (op == OP_LBU || op == OP_LHU || op == OP_LB || op == OP_LH)
    this->emitLoadSubroutine(op);
  else /* op == OP_SB || op == OP_SH */
    this->emitStoreSubroutine(op);
}

void JavaMethod::emitSubroutines()
{
  uint8_t is_emitted[N_INSNS];

  memset(is_emitted, 0, sizeof(is_emitted));

  /* If any of LB/LBU, LH/LHU, SB,SH, emit subroutines to handle
   * these */
  for (int i = 0; i < this->n_functions; i++)
    {
      mips_opcode_t ops_to_check[] = {OP_LB, OP_LBU, OP_LH, OP_LHU, OP_SB, OP_SH};

      for (unsigned int j = 0; j < sizeof(ops_to_check) / sizeof(mips_opcode_t); j++)
        {
          mips_opcode_t op = ops_to_check[j];

          if ( !is_emitted[op] && this->functions[i]->opcodeIsUsed(op) )
            {
              this->emitSubroutineForOp(op);
              is_emitted[op] = 1;
            }
        }
    }
}

bool JavaMethod::pass2()
{
  bool out = true;

  regalloc->setAllocation(this->registerUsage);

  emit->generic("\n.method public static %s\n"
                ".limit stack %d\n"
                ".limit locals %d\n",
                this->getJavaMethodName(),
                this->getMaxStackHeight() + 2,
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
      panic_if(i == R_FNA && this->registerUsage[i] > 0 && !this->hasMultipleFunctions(),
               "%s is a single-function method but uses R_FNA %d times\n",
               this->getName(), this->registerUsage[i]);

      /* These are passed as arguments, skip them */
      if (this->registerIsArgument((MIPS_register_t)i))
        continue;

      /* If this register is used, initialize it */
      if (this->registerUsage[i] > 0)
        {
          /* Multi-function classes assign to RA */
          if (this->hasMultipleFunctions() && i == R_RA)
            emit->bc_pushconst(-1);
          else if (i == R_MEM)
            emit->bc_getstatic( "%sCRunTime/memory [I",
                controller->getJasminPackagePath());
          else
            emit->bc_pushconst(0);
          emit->bc_popregister((MIPS_register_t)i);
        }
    }

  /* If we are configured for it, generate jsr targets for lb/lh
   * etc */
  if (config->optimizePartialMemoryOps)
    {
      emit->bc_goto("__CIBYL_javamethod_begin");
      this->emitSubroutines();
    }

  emit->bc_label("__CIBYL_javamethod_begin");

  if (this->hasMultipleFunctions())
    {
      uint32_t *table = (uint32_t*)xcalloc(this->n_functions,
                                           sizeof(uint32_t));

      /* Setup a table of functions to jump to */
      for (int i = 0; i < this->n_functions; i++)
        table[i] = this->functions[i]->getAddress();

      emit->bc_pushregister(R_FNA);
      emit->bc_tableswitch(0, this->n_functions,
                           table, "__CIBYL_function_return");

      free(table);
    }

  for (int i = 0; i < this->n_functions; i++)
    {
      Function *fn = this->functions[i];

      /* And compile the function */
      if (this->hasMultipleFunctions())
        emit->bc_label( fn->getAddress() );
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

  emit->bc_label("__CIBYL_exception_handlers");
  for (int i = 0; i < this->n_exceptionHandlers; i++)
    {
      ExceptionHandler *eh = this->exceptionHandlers[i];
      
      if (eh->pass2() != true)
        out = false;
    }

  emit->bc_label("__CIBYL_function_return");
  if (this->hasMultipleFunctions() && this->n_returnLocations > 0)
    {
      emit->bc_pushregister(R_RA);
      emit->bc_tableswitch(0, this->n_returnLocations, this->returnLocations,
                           "__CIBYL_function_return_non_local");
      emit->bc_label("__CIBYL_function_return_non_local");
    }

  if (config->threadSafe)
    {
      if (this->returnSize() == 2)
        {
          /* Return a 64-bit value */
          emit->bc_pushregister(R_V1);
          emit->bc_i2l();
          emit->bc_pushconst(32);
          emit->bc_lshl();
          emit->bc_pushregister(R_V0);
          emit->bc_i2l();
          emit->bc_lor(); /* push (v1 << 32) | v0 */
          emit->bc_lreturn();
        }
      else if (this->returnSize() == 1) /* Only v0 */
        {
          emit->bc_pushregister(R_V0);
          emit->bc_ireturn();
        }
      else
        emit->bc_return();
    }
  else
    {
      /* Not thread safe */
      if (this->returnSize() == 2)
        {
          emit->bc_pushregister(R_V1);
          emit->bc_putstatic("%sCRunTime/saved_v1 I",
              controller->getJasminPackagePath());
        }
      if (this->returnSize() >= 1)
        {
          emit->bc_pushregister(R_V0);
          emit->bc_ireturn();
        }
      else
        emit->bc_return();
    }

  emit->generic(".end method ; %s\n", this->getJavaMethodName());

  return out;
}

MIPS_register_t JavaMethod::getFirstRegisterToPass(void *_it)
{
  int *it = (int*)_it;

  *it = 0;
  return this->getNextRegisterToPass(it);
}

MIPS_register_t JavaMethod::getNextRegisterToPass(void *_it)
{
  int *it = (int*)_it;
  MIPS_register_t ret;

  while (this->m_possibleArguments[*it] != R_ZERO &&
	 this->clobbersReg(this->m_possibleArguments[*it] ) == 0)
      (*it)++;
  ret = this->m_possibleArguments[*it];
  (*it)++;
  return ret;
}

bool JavaMethod::registerIsArgument(MIPS_register_t reg)
{
  int i;

  for (i = 0; this->m_possibleArguments[i] != R_ZERO; i++)
    {
      if (this->m_possibleArguments[i] == reg)
        return true;
    }

  return false;
}

int JavaMethod::returnSize()
{
  panic_if(this->m_returnSize < 0 || this->m_returnSize > 2,
      "returnSize accessed too soon for %s %d\n",
      this->getJavaMethodName(), this->m_returnSize);
  return this->m_returnSize;
}

void JavaMethod::setReturnSize(int n)
{
  panic_if(n < 0 || n > 2,
      "return size is out of bounds for %s: %d\n",
      this->getJavaMethodName(), n);

  this->m_returnSize = n;
}

bool JavaMethod::clobbersReg(MIPS_register_t reg)
{
  return this->registerUsage[reg] > 0;
}

char *JavaMethod::addExceptionHandler(ExceptionHandler *eh)
{
  int n = this->n_exceptionHandlers;

  this->exceptionHandlers = (ExceptionHandler**)xrealloc(this->exceptionHandlers,
                                                         (n + 1) * sizeof(ExceptionHandler*) );
  this->n_exceptionHandlers = n + 1;
  this->exceptionHandlers[n] = eh;

  return eh->name;
}
