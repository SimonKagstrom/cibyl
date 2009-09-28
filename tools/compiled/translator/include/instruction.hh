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

  virtual size_t getMaxStackHeight()
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

  bool hasDelayed()
  {
    return this->delayed != NULL;
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

  MIPS_register_t getRegister(mips_register_type_t type)
  {
    switch(type)
    {
    case I_RS:
      return this->getRs();
    case I_RT:
      return this->getRt();
    case I_RD:
      return this->getRd();
    default:
      panic("getRegister called with type %d\n", type);
    }
  }

  int32_t getExtra() { return this->extra; }

  /**
   * Get the last instruction where the register @a which was written to
   * in the basic block of this instruction
   *
   * @param which the register to lookup (rs/rt/rd)
   * @return a pointer to the last instruction or NULL if there
   *         was no previous write
   */
  Instruction *getPrevRegisterWrite(mips_register_type_t which)
  {
    return this->prev_register_writes[which];
  }

  /**
   * Get the last instruction where the register @a which was read from
   * in the basic block of this instruction
   *
   * @param which the register to lookup (rs/rt/rd)
   * @return a pointer to the last instruction or NULL if there
   *         was no previous read
   */
  Instruction *getPrevRegisterRead(mips_register_type_t which)
  {
    return this->prev_register_reads[which];
  }

  void setPrevRegisterReadAndWrite(Instruction *rinsn, Instruction *winsn,
      mips_register_type_t which)
  {
    this->setPrevNextRegisterGeneric(this->prev_register_reads, which, rinsn);
    this->setPrevNextRegisterGeneric(this->prev_register_writes, which, winsn);
  }

  void setNextRegisterReadAndWrite(Instruction *rinsn, Instruction *winsn,
      mips_register_type_t which)
  {
    this->setPrevNextRegisterGeneric(this->next_register_reads, which, rinsn);
    this->setPrevNextRegisterGeneric(this->next_register_writes, which, winsn);
  }

  BasicBlock *parent;
protected:
  void setPrevNextRegisterGeneric(Instruction **table, mips_register_type_t which,
      Instruction *insn)
  {
    table[which] = insn;
  }

  int opcode;
  MIPS_register_t rs, rt, rd;
  int32_t extra;
  Instruction *delayed;
  Instruction *prefix;

  /* rs/rt/rd */
  Instruction *prev_register_writes[3];
  Instruction *prev_register_reads[3];
  Instruction *next_register_writes[3];
  Instruction *next_register_reads[3];
 
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
