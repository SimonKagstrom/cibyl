/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      move.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Describes a move
 *
 * $Id: move.h 11605 2006-10-12 10:19:04Z ska $
 *
 ********************************************************************/
#ifndef __MOVE_H__
#define __MOVE_H__

#include "config.h"
#include "utils.h"
#include "item.h"

#define MOVE_BUFLEN 6

typedef struct
{
  int sx, sy;
  int dx, dy;
  item_t who;
} move_t;

static inline void move_init(move_t *p_dst, item_t who, int sx, int sy, int dx, int dy)
{
  p_dst->who = who;
  p_dst->sx = sx;
  p_dst->sy = sy;
  p_dst->dx = dx;
  p_dst->dy = dy;
}

static inline move_t move_new(item_t who, int sx, int sy, int dx, int dy)
{
  move_t out;

  move_init(&out, who, sx, sy, dx, dy);

  return out;
}

extern bool_t move_to_string(move_t *p_src, char *dst, int buflen);
extern bool_t move_from_string(move_t *p_dst, char *src, int buflen);

#endif /* !__MOVE_H__ */
