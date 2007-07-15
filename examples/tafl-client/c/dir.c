/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      dir.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Direction stuff
 *
 * $Id: dir.c 11477 2006-10-06 14:17:27Z ska $
 *
 ********************************************************************/
#include "dir.h"

int dir_to_dx(dir_t dir)
{
  if (dir == LEFT || dir == UP_LEFT || dir == DOWN_LEFT)
    return -1;
  if (dir == RIGHT || dir == UP_RIGHT || dir == DOWN_RIGHT)
    return 1;
  return 0;
}

int dir_to_dy(dir_t dir)
{
  if (dir == UP || dir == UP_LEFT || dir == UP_RIGHT)
    return -1;
  if (dir == DOWN || dir == DOWN_LEFT || dir == DOWN_RIGHT)
    return 1;
  return 0;
}

dir_t dx_dy_to_dir(int sx, int sy, int dx, int dy)
{
  if (dx - sx < 0)
    return LEFT;
  else if (dx - sx > 0)
    return RIGHT;
  else if (dy - sy < 0)
    return UP;
  else if (dy - sy > 0)
    return DOWN;

  /* Only get here if sx==sy && dx == dy */
  return -1;
}
