/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      stat.h
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Stat (for mkdir)
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __SYS__STAT_H__
#define __SYS__STAT_H__

typedef int mode_t; /* Actually ignored */

extern int mkdir(const char *pathname, mode_t mode);

#endif /* !__SYS__STAT_H__ */
