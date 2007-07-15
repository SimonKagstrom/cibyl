/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      dir.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Directions
 *
 * $Id: dir.h 11477 2006-10-06 14:17:27Z ska $
 *
 ********************************************************************/
#ifndef __DIR_H__
#define __DIR_H__

#define LEFT           0
#define RIGHT          1
#define UP             2
#define DOWN           3
#define UP_LEFT        4
#define UP_RIGHT       5
#define DOWN_LEFT      6
#define DOWN_RIGHT     7

typedef int dir_t;

extern int dir_to_dx(dir_t dir);
extern int dir_to_dy(dir_t dir);
extern dir_t dx_dy_to_dir(int sx, int sy, int dx, int dy);

#endif /* !__DIR_H__ */
