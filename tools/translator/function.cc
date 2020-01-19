/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      function.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Function implementation
 *
 * $Id:$
 *
 ********************************************************************/
#include <string.h>
#include <stdlib.h>
#include <utils.h>
#include <function.hh>

#include <algorithm>
using namespace std; // for std::max

Function::Function(const char *name, Instruction **insns,
		   int first_insn, int last_insn) : CodeBlock()
{
  bb_type_t type = PROLOGUE;
  int n_bbs = 0;
  int first = 0;
  int last = 0;

  memset(this->registerSources, 0, sizeof(this->registerSources));
  memset(this->registerDestinations, 0, sizeof(this->registerDestinations));
  memset(this->usedInsns, 0, sizeof(this->usedInsns));

  this->bbs = (BasicBlock**)xcalloc(1, sizeof(BasicBlock*));
  this->address = insns[first_insn]->getAddress();
  this->size = 0;
  this->registerIndirectJumps = false;

  this->realName = xstrdup(name);
  this->name = (char*)xcalloc(strlen(name) + 16, 1);

  char *cpy = xstrdup(name);

  /* Fixup GCC temporary function names, e.g., T.35 etc */
  int o = 0;
  for (; cpy[o]; o++)
    {
      char c = cpy[o];

      if (c == '.')
        c = '_';
      cpy[o] = c;
    }
  xsnprintf(this->name, strlen(cpy) + 16, "%s_%x", cpy, this->getAddress());

  /* Create basic blocks */
  for (int i = first_insn; i < last_insn; i++)
    {
      Instruction *insn = insns[i];

      if (insn->isRegisterIndirectJump())
        this->registerIndirectJumps = true;

      /* Mark this opcode as used */
      this->markOpcodeUsed(insn->getOpcode());
      if (insn->hasDelaySlot())
        this->markOpcodeUsed(insn->getDelayed()->getOpcode());
      if (insn->hasPrefix())
        this->markOpcodeUsed(insn->getPrefix()->getOpcode());

      last = last + 1;
      if ( insn->isReturnJump() )
        {
          type = EPILOGUE;
        }
      if ( (insn->isBranch() || insn->isBranchTarget()) &&
	   !(insn->getAddress() == this->address) )
	{
	  if (insn->hasDelaySlot() && i + 1 < last_insn)
	    {
	      /* If this instruction has a delay slot, also add the
	       * delay slot instruction */
	      last++;
	      i++;
	    }
	  if (last != first)
	    {
	      this->bbs[n_bbs] = new BasicBlock(insns, type, first_insn + first,
						first_insn + last);
	      this->size += this->bbs[n_bbs]->getSize();
	      n_bbs++;
	      this->bbs = (BasicBlock**)xrealloc(this->bbs, (n_bbs+1) * sizeof(BasicBlock*));
	      this->bbs[n_bbs] = NULL;
	    }
	  first = last;

          /* After the first, we now have normal bb's */
          type = NORMAL;
	}
    }
  /* Create the last bb */
  if (last != first)
    {
      this->bbs[n_bbs] = new BasicBlock(insns, NORMAL, first_insn + first,
					last_insn);
      this->size += this->bbs[n_bbs]->getSize();
      n_bbs++;
    }
  this->n_bbs = n_bbs;

  /* Fixup the bytecode size */
  this->bc_size = 0;
  this->maxStackHeight = 0;
  for (int i = 0; i < this->n_bbs; i++)
    this->bc_size += this->bbs[i]->getBytecodeSize();
}

Function::~Function()
{
  free(this->name);
}

bool Function::pass1()
{
  for (int i = 0; i < this->n_bbs; i++)
    {
      BasicBlock *bb = this->bbs[i];

      /* Run the pass1 of the bb and the instructions */
      bb->pass1();

      /* Fill in the register usage of this function */
      bb->fillDestinations(this->registerDestinations);
      bb->fillSources(this->registerSources);
      this->maxStackHeight = max(this->maxStackHeight, bb->getMaxStackHeight());
    }

  return true;
}

bool Function::pass2()
{
  for (int i = 0; i < this->n_bbs; i++)
    {
      BasicBlock *bb = this->bbs[i];

      if (!bb->pass2())
	return false;
    }
  return true;
}

int Function::fillDestinations(int *p)
{
  for (int i = 0; i < N_REGS; i++)
    p[i] += this->registerDestinations[i];

  return 1;
}

int Function::fillSources(int *p)
{
  for (int i = 0; i < N_REGS; i++)
    p[i] += this->registerSources[i];

  return 0;
}


StartFunction::StartFunction(const char *name, Instruction **insns,
                             int first_insn, int last_insn) : Function(name, insns, first_insn, last_insn)
{
  free(this->name);
  this->name = (char*)"start";
}

bool StartFunction::pass1()
{
  for (int i = 0; i < this->n_bbs; i++)
    {
      BasicBlock *bb = this->bbs[i];

      /* Run the pass1 of the bb and the instructions */
      bb->pass1();

      /* Fill in the register usage of this function */
      bb->fillDestinations(this->registerDestinations);
      bb->fillSources(this->registerSources);
      this->maxStackHeight = max(this->maxStackHeight, bb->getMaxStackHeight());
    }

  /* Start always have everything defined */
  this->registerDestinations[ R_SP ]++;
  this->registerDestinations[ R_A0 ]++;
  this->registerDestinations[ R_A1 ]++;
  this->registerDestinations[ R_A2 ]++;
  this->registerDestinations[ R_A3 ]++;
  this->registerDestinations[ R_V0 ]++;
  this->registerDestinations[ R_V1 ]++;

  return true;
}
