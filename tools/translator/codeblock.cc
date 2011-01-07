/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      codeblock.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   CodeBlock impl.
 *
 * $Id:$
 *
 ********************************************************************/
#include <stdio.h>

#include <entity.hh>
#include <utils.h>

void CodeBlock::addJumptabLabel(uint32_t addr)
{
  int n = this->n_jumptabLabels;

  for (int i = 0; i < n; i++)
    {
      /* Don't add the same twice */
      if (this->jumptabLabels[i] == addr)
        return;
    }

  this->n_jumptabLabels++;

  this->jumptabLabels = (uint32_t*)xrealloc(this->jumptabLabels, this->n_jumptabLabels * sizeof(uint32_t));
  this->jumptabLabels[n] = addr;
}

static int uint32_cmp(const void *_a, const void *_b)
{
  uint32_t a = *(uint32_t*)_a;
  uint32_t b = *(uint32_t*)_b;

  return a - b;
}

void CodeBlock::sortJumptabLabels()
{
  qsort(this->jumptabLabels, this->n_jumptabLabels, sizeof(uint32_t), uint32_cmp);
}

bool CodeBlock::hasJumptabLabel(uint32_t addr)
{
  uint32_t tmp = addr;

  return bsearch(&tmp, this->jumptabLabels, this->n_jumptabLabels,
                 sizeof(uint32_t), uint32_cmp) != NULL;
}
