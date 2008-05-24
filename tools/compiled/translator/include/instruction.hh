/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      instruction.hh
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Instruction class
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __INSTRUCTION_HH__
#define __INSTRUCTION_HH__

#include <mips.hh>
#include <registerallocator.hh>
#include <syscall.hh>
#include <entity.hh>

class JavaClass;
class BasicBlock;
class Function;

class Instruction : public Entity
{
public:
  Instruction(uint32_t address, int opcode,
	      MIPS_register_t rs, MIPS_register_t rt, MIPS_register_t rd, int32_t extra);

  virtual ~Instruction();

  virtual bool isBranch()
  {
    return false;
  }

  virtual bool isReturnJump()
  {
    return false;
  }

  virtual bool isRegisterIndirectJump()
  {
    return false;
  }

  mips_opcode_t getOpcode()
  {
    return (mips_opcode_t)this->opcode;
  }

  bool isBranchTarget()
  {
    return this->branchTarget;
  }

  void setBranchTarget();

  virtual bool isNop()
  {
    return false;
  }

  virtual bool pass1() = 0;

  virtual bool pass2() = 0;

  virtual int getMaxStackHeight()
  {
    return 2;
  }

  virtual bool hasDelaySlot()
  {
    return this->isBranch();
  }

  /* Delay slot instructions are "appended" to the parent
   * instruction. A special class denotes delay slot nops to handle
   * e.g., labels on delay slots */
  virtual bool isDelaySlotNop()
  {
    return false;
  }

  /**
   * Fill in the register destinations
   *
   * @param p the destinations to fill in
   *
   * @return the number of destinations filled
   */
  virtual int fillDestinations(int *p) { return 0; };

  virtual int fillSources(int *p) { return 0; };

  void setDelayed(Instruction *delayed)
  {
    this->delayed = delayed;
  }

  Instruction *getDelayed()
  {
    return this->delayed;
  }

  void setPrefix(Instruction *prefix)
  {
    this->prefix = prefix;
  }

  bool hasPrefix()
  {
    return this->prefix != NULL;
  }

  Instruction *getPrefix()
  {
    return this->prefix;
  }

  MIPS_register_t getRs() { return this->rs; }

  MIPS_register_t getRt() { return this->rt; }

  MIPS_register_t getRd() { return this->rd; }

  int32_t getExtra() { return this->extra; }

  BasicBlock *parent;
protected:
  int opcode;
  MIPS_register_t rs, rt, rd;
  int32_t extra;
  Instruction *delayed;
  Instruction *prefix;

  bool branchTarget;
};

class InstructionFactory
{
public:
  static InstructionFactory *getInstance();

  Instruction *create(uint32_t address, uint32_t encoding);

  Instruction *createNop(uint32_t address);

  Instruction *createDelaySlotNop(uint32_t address);

  Instruction *createTailCallJump(uint32_t address, uint32_t extra);

private:
  static InstructionFactory *instance;
};

int instruction_to_string(Instruction *insn, char *buf, int buf_len);

#endif /* !__INSTRUCTION_HH__ */
