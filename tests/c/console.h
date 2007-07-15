/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      console.h
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Console defs
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __CONSOLE_H__
#define __CONSOLE_H__

void console_push(char *str);

void console_redraw(void);

void console_init(void);

#endif /* !__CONSOLE_H__ */
