/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      entity.hh
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Base class for almost everything
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __ENTITY_HH__
#define __ENTITY_HH__

#include <stdint.h>
#include <stdlib.h>

#include <mips.hh>

class Entity
{
public:
  Entity()
  {
    this->address = 0;
    this->size = 0;
  }

  uint32_t getAddress()
  {
    return this->address;
  }

  uint32_t getSize()
  {
    return this->size;
  }

  int addToRegisterUsage(MIPS_register_t reg, int *p)
  {
    if (reg == 0)
      return 0;
    p[reg]++;

    return 1;
  }

protected:
  uint32_t address;
  size_t size;
};


class CodeBlock : public Entity
{
public:
  CodeBlock() : Entity()
  {
    this->jumptabLabels = NULL;
    this->n_jumptabLabels = 0;
  }

  void addJumptabLabel(uint32_t addr);

  bool hasJumptabLabel(uint32_t addr);

  void sortJumptabLabels();

protected:
  uint32_t *jumptabLabels;
  int n_jumptabLabels;
};

#endif /* !__ENTITY_HH__ */
