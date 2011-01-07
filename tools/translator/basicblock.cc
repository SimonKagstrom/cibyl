/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      basicblock.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Basic block implementation
 *
 * $Id:$
 *
 ********************************************************************/
#include <controller.hh>
#include <basicblock.hh>
#include <emit.hh>
#include <config.hh>

BasicBlock::BasicBlock(Instruction **insns,
		       bb_type_t type,
		       int first, int last) : CodeBlock()
{
  panic_if(first < 0 || last < 0 || first > last,
           "Basic block constructed with wrong instructions %d to %d\n", first, last);

  this->instructions = &(insns[first]);
  this->n_insns = last - first;
  this->type = type;

  this->address = this->instructions[0]->getAddress();
  this->size = this->n_insns * 4;

  /* Fixup the bytecode size and fill in last register writes and
   * last register reads for the instruction */
  this->bc_size = 0;
  this->maxStackHeight = 0; /* Fixed in pass 1 */
  for (int i = 0; i < n_insns; i++)
    {
      Instruction *insn = this->instructions[i];

      /* Order is important. The parameters for the instruction with a delay
       * slot is pushed before the delay slot */
      if (insn->hasPrefix())
        this->bc_size += insn->getPrefix()->getBytecodeSize();
      this->bc_size += insn->getBytecodeSize();
      if (insn->hasDelayed())
        this->bc_size += insn->getDelayed()->getBytecodeSize();
    }
}

void BasicBlock::handleTables(Instruction **rtab, Instruction **wtab,
    Instruction *insn, bool before)
{
  /* First update the instruction and then the tables */
  this->fillInstructionReadsAndWrites(rtab, wtab, insn, before);
  /* Update the tables with this instruction */
  this->fillReadTable(rtab, insn);
  this->fillWriteTable(wtab, insn);
}

void BasicBlock::handleRegisterTablesPrev(Instruction **rtab, Instruction **wtab,
    Instruction *insn)
{
  this->handleTables(rtab, wtab, insn, true);
}

void BasicBlock::handleRegisterTablesAfter(Instruction **rtab, Instruction **wtab,
    Instruction *insn)
{
  this->handleTables(rtab, wtab, insn, false);
}

void BasicBlock::fillInstructionReadsAndWrites(Instruction **rtab,
    Instruction **wtab, Instruction *insn, bool before)
{
  mips_register_type_t regs[3] = {I_RS, I_RT, I_RD};
  int uses[N_REGS];

  memset(uses, 0, sizeof(uses));
  insn->fillDestinations(uses);
  insn->fillSources(uses);

  for (unsigned i = 0; i < sizeof(regs) / sizeof(mips_register_type_t); i++)
    {
      MIPS_register_t reg = insn->getRegister(regs[i]);

      if (uses[reg])
        {
          if (before)
            insn->setPrevRegisterReadAndWrite(rtab[reg],
                wtab[reg], regs[i]);
          else
            insn->setNextRegisterReadAndWrite(rtab[reg],
                wtab[reg], regs[i]);
        }
    }
}

void BasicBlock::fillWriteTable(Instruction **table, Instruction *insn)
{
  int dsts[N_REGS];
  
  memset(dsts, 0, sizeof(dsts));
  insn->fillDestinations(dsts);

  /* rs is rarely used as a destination, but we want to be sure! */
  if (dsts[insn->getRs()])
    table[insn->getRs()] = insn;
  if (dsts[insn->getRt()])
    table[insn->getRt()] = insn;
  if (dsts[insn->getRd()])
    table[insn->getRd()] = insn;
}

void BasicBlock::fillReadTable(Instruction **table, Instruction *insn)
{
  int srcs[N_REGS];
  
  memset(srcs, 0, sizeof(srcs));
  insn->fillSources(srcs);

  if (srcs[insn->getRs()])
    table[insn->getRs()] = insn;
  if (srcs[insn->getRt()])
    table[insn->getRt()] = insn;
  if (srcs[insn->getRd()])
    table[insn->getRd()] = insn;
}

bool BasicBlock::pass1()
{
  bool out = true;
  Instruction **write_table = (Instruction**)xcalloc(N_REGS,
      sizeof(Instruction*));
  Instruction **read_table = (Instruction**)xcalloc(N_REGS,
      sizeof(Instruction*));

  for (int i = 0; i < this->n_insns; i++)
    {
      Instruction *insn = this->instructions[i];

      controller->setCurrentInstruction(insn);

      if (insn->getOpcode() == OP_J)
        {
          uint32_t dstAddress = insn->getExtra() << 2;
          JavaMethod *src = controller->getMethodByAddress(insn->getAddress());
          JavaMethod *dst = controller->getMethodByAddress(dstAddress);

          if (src != dst)
            {
              /* Typically tail call optimizations, just "unoptimize" */
              emit->warning("Jump from 0x%x to 0x%x not within the same method, replacing with jal\n",
                            this->address, dstAddress);
              this->instructions[i] = InstructionFactory::getInstance()->createTailCallJump(insn->getAddress(),
                                                                                            insn->getExtra());
              if (insn->isBranchTarget())
                this->instructions[i]->setBranchTarget();
              this->instructions[i]->setDelayed(insn->getDelayed());
              delete insn;
              insn = this->instructions[i];
            }
        }

      /* Prepare the instruction */
      if (!insn->pass1())
	out = false;

      /* Order is important. The parameters for the instruction with a delay
       * slot is pushed before the delay slot */
      if (insn->hasPrefix())
        this->handleRegisterTablesPrev(read_table, write_table, insn->getPrefix());
      this->handleRegisterTablesPrev(read_table, write_table, insn);
      if (insn->hasDelayed())
        this->handleRegisterTablesPrev(read_table, write_table, insn->getDelayed());
    }

  /* Fill in the tables in reverse (next instruction write) */
  memset(write_table, 0, sizeof(Instruction*) * N_REGS);
  memset(read_table, 0, sizeof(Instruction*) * N_REGS);
  for (int i = this->n_insns - 1; i >= 0; i--)
    {
      Instruction *insn = this->instructions[i];

      if (insn->hasPrefix())
        this->handleRegisterTablesAfter(read_table, write_table, insn->getPrefix());
      this->handleRegisterTablesAfter(read_table, write_table, insn);
      if (insn->hasDelayed())
        this->handleRegisterTablesAfter(read_table, write_table, insn->getDelayed());
    }

  free(write_table);
  free(read_table);

  for (int i = 0; i < this->n_insns; i++)
    {
      Instruction *insn = this->instructions[i];

      if (config->optimizePruneStackStores &&
          ((this->type == PROLOGUE && insn->getOpcode() == OP_SW && insn->getRs() == R_SP) ||
              (this->type == EPILOGUE && insn->getOpcode() == OP_LW && insn->getRs() == R_SP)) )
        {
          int p_s[N_REGS];
          int p_d[N_REGS];

          memset(p_s, 0, sizeof(p_s));
          memset(p_d, 0, sizeof(p_d));
          insn->fillSources(p_s);
          insn->fillDestinations(p_d);

          /* Loop through all caller saved registers and for writes, simply skip them */
          for (int j = 0; mips_caller_saved[j] != R_ZERO; j++)
            {
              MIPS_register_t reg = mips_caller_saved[j];

              if ( (this->type == PROLOGUE && p_s[reg] != 0 && !insn->getPrevRegisterWrite(I_RT)) ||
                  (this->type == EPILOGUE && p_d[reg] != 0))
                {
                  this->instructions[i] = InstructionFactory::getInstance()->createNop(insn->getAddress());
                  if (insn->isBranchTarget())
                    this->instructions[i]->setBranchTarget();
                  delete insn;
                  insn = this->instructions[i];
                  break;
                }
            }
        }
      this->maxStackHeight = max(this->maxStackHeight, insn->getMaxStackHeight());
      if (insn->getDelayed())
        this->maxStackHeight = max(this->maxStackHeight,
            insn->getDelayed()->getMaxStackHeight());
    }

  return out;
}

static void pushRegister(MIPS_register_t reg)
{
  if (reg != R_RA)
    emit->bc_pushregister(reg);
  else
    emit->bc_pushconst(0);
}

void BasicBlock::traceRegisterValues(Instruction *insn)
{
  MIPS_register_t rs,rt,rd;

  rs = insn->getRs();
  rt = insn->getRt();
  rd = insn->getRd();

  /* Push all registers, then invoke the tracer */
  pushRegister(rs);
  pushRegister(rt);
  pushRegister(rd);

  emit->bc_invokestatic("%sCRunTime/emitRegisterTrace(III)V",
      controller->getJasminPackagePath());
  if (insn->getDelayed())
    this->traceRegisterValues(insn->getDelayed());
}

Instruction *BasicBlock::lookupPrevRegister(Instruction *insn, MIPS_register_t reg,
    bool is_write)
{
  mips_register_type_t which;

  if (reg == insn->getRs())
    which = I_RS;
  else if (reg == insn->getRt())
    which = I_RT;
  else if (reg == insn->getRd())
    which = I_RD;
  else /* Does not access the register! */
    return NULL;

  if (is_write)
    return insn->getPrevRegisterWrite(which);
  return insn->getPrevRegisterRead(which);
}

void BasicBlock::commentInstruction(Instruction *insn)
{
  char buf[255];
  char out[255];

  memset(buf, 0, 255);
  memset(out, 0, 255);

  /* Output a comment */
  int l = snprintf(buf, 255, "0x%x: ", insn->getAddress());
  l += instruction_to_string(insn, buf+l, 255);
  if (insn->hasDelaySlot())
    {
      buf[l] = ' ';
      instruction_to_string(insn->getDelayed(), buf+l+1, 255-l-1);
    }

  xsnprintf(out, 255, "0x%08x: %s", insn->getAddress(), buf);
  emit->bc_comment(out);

  if ( insn->getAddress() >= config->traceRange[0] && insn->getAddress() < config->traceRange[1])
    {
      emit->bc_ldc(buf);
      emit->bc_invokestatic("%sCRunTime/emitTrace(Ljava/lang/String;)V",
          controller->getJasminPackagePath());
      this->traceRegisterValues(insn);
    }
}

bool BasicBlock::pass2()
{
  bool out = true;

  emit->bc_comment("--- basic block ---");
  for (int i = 0; i < this->n_insns; i++)
    {
      Instruction *insn = this->instructions[i];

      controller->setCurrentInstruction(insn);

      /* FIXME: Emit .line info if debug is on */
      if ( (insn->isBranchTarget() || controller->hasJumptabLabel(insn->getAddress())) &&
           !insn->isDelaySlotNop() )
	emit->bc_label( insn->getAddress() );

      if (!insn->isNop())
	this->commentInstruction(insn);

      /* Compile the instruction */
      if (!insn->pass2())
	out = false;

      if ( insn->hasDelaySlot() && controller->hasJumptabLabel(insn->getDelayed()->getAddress()) )
        {
          uint32_t addr = insn->getDelayed()->getAddress();

          emit->warning("Instruction on address 0x%x is in delay slot but has a label\n",
                        addr);
          /* Jump past the instruction and emit it again */
          emit->bc_goto("L_%x_out", addr);

          /* Emit the instruction again */
          emit->bc_label("L_%x", addr);
          insn->getDelayed()->pass2();
          emit->bc_label("L_%x_out", addr);
        }
    }

  return out;
}


int BasicBlock::fillDestinations(int *p)
{
  int out = 0;

  for (int i = 0; i < this->n_insns; i++)
    {
      Instruction *insn = this->instructions[i];
      Instruction *delayed = insn->getDelayed();
      Instruction *prefix = insn->getPrefix();

      out += insn->fillDestinations(p);
      if (delayed)
        out += delayed->fillDestinations(p);
      if (prefix)
        out += prefix->fillDestinations(p);
    }

  return out;
}

int BasicBlock::fillSources(int *p)
{
  int out = 0;

  for (int i = 0; i < this->n_insns; i++)
    {
      Instruction *insn = this->instructions[i];
      Instruction *delayed = insn->getDelayed();
      Instruction *prefix = insn->getPrefix();

      out += insn->fillSources(p);
      if (delayed)
        out += delayed->fillSources(p);
      if (prefix)
        out += prefix->fillSources(p);
    }

  return out;
}
