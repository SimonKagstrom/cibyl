/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      view.h
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Android API definition
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __ANDROID__VIEW_H__
#define __ANDROID__VIEW_H__

#include <cibyl.h>
#include <stdlib.h> /* NOPH_registerCallback */
#include <android/graphics.h> /* NOPH_Canvas_t */

typedef int NOPH_KeyEvent_t;
typedef int NOPH_View_t;

/**
 * Register a callback for the onDraw event. This is the same as
 * implementing View::onDraw() in Java.
 *
 * @param fn the callback function to call on a redraw operation. Pass
 *           NULL to disable the callback
 */
static inline void NOPH_View_registerOnDrawCallback(void (*fn)(NOPH_Canvas_t canvas))
{
  NOPH_registerCallback("android.view.View.onDraw", (int)fn );
}

/** @see NOPH_View_registerOnDrawCallback */
static inline void NOPH_View_registerOnKeyDownCallback(int (*fn)(int keyCode, NOPH_KeyEvent_t msg))
{
  NOPH_registerCallback("android.view.View.onKeyDown", (int)fn );
}

/** @see NOPH_View_registerOnDrawCallback */
static inline void NOPH_View_registerOnKeyUpCallback(int (*fn)(int keyCode, NOPH_KeyEvent_t msg))
{
  NOPH_registerCallback("android.view.View.onKeyUp", (int)fn );
}


#endif /* !__ANDROID__VIEW_H__ */
