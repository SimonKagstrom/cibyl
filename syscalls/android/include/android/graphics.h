/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      graphics.h
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Android API definition
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __ANDROID__GRAPHICS_H__
#define __ANDROID__GRAPHICS_H__

#include <cibyl.h>

#define NOPH_Color_BLUE 0xff0000ff

typedef int NOPH_Paint_t;
typedef int NOPH_Canvas_t;
typedef int NOPH_Color_t;

void NOPH_Canvas_drawLine(NOPH_Canvas_t canvas, float startX, float startY, float stopX, float stopY, NOPH_Paint_t paint);

NOPH_Paint_t NOPH_Paint_new(void);
void NOPH_Paint_setColor(NOPH_Paint_t paint, int color);

#endif /* !__ANDROID__GRAPHICS_H__ */
