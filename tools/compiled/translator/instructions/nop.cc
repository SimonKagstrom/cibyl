/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      nop.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Nops
 *
 * $Id:$
 *
 ********************************************************************/

class Nop : public Instruction
{
public:
  Nop(uint32_t address) : Instruction(address, 0, R_ZERO, R_ZERO, R_ZERO, 0)
  {
  }

  virtual bool isNop()
  {
    return true;
  }

  bool pass1()
  {
    return true;
  }

  bool pass2()
  {
    return true;
  }

  int toString(char *dst, size_t n = 255)
  {
    return snprintf(dst, n, " ");
  }
};



class DelaySlotNop : public Nop
{
public:
  DelaySlotNop(uint32_t address) : Nop(address)
  {
  }

  bool isDelaySlotNop()
  {
    return true;
  }
};
