/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      move.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Implementation of some move-related stuff
 *
 * $Id: move.c 11837 2006-11-01 12:10:18Z ska $
 *
 ********************************************************************/
#include "move.h"

bool_t move_to_string(move_t *p_src, char *dst, int buflen)
{
  if (buflen < MOVE_BUFLEN)
    return FALSE;

  strncpy(dst, "MOVE", buflen);
  dst += 4;
  int_to_charbuf(p_src->who, dst + 1, 1);
  int_to_charbuf(p_src->sx, dst + 2, 1);
  int_to_charbuf(p_src->sy, dst + 3, 1);
  int_to_charbuf(p_src->dx, dst + 4, 1);
  int_to_charbuf(p_src->dy, dst + 5, 1);

  return TRUE;
}

bool_t move_from_string(move_t *p_dst, char *src, int buflen)
{
  if (buflen < MOVE_BUFLEN || strncmp(src, "MOVE", buflen) != 0)
    return FALSE;

  p_dst->who = (item_t)charbuf_to_int(src + 1, 1);
  p_dst->sx  = charbuf_to_int(src + 2, 1);
  p_dst->sy  = charbuf_to_int(src + 3, 1);
  p_dst->dx  = charbuf_to_int(src + 4, 1);
  p_dst->dy  = charbuf_to_int(src + 5, 1);

  if (p_dst->who == PF_WHITE || p_dst->who == PF_BLACK)
    return FALSE;

  return TRUE;
}
