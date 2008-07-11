/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      item.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Item impl.
 *
 * $Id: item.c 11478 2006-10-06 14:20:35Z ska $
 *
 ********************************************************************/
#include "item.h"

item_t COLOR(item_t item)
{
  if (item == PF_WHITE || item == PF_WHITE_KING)
    return PF_WHITE;
  if (item == PF_BLACK)
    return PF_BLACK;

  return PF_INVALID;
}
