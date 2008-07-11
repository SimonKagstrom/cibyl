/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      main.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   J2ME test app
 *
 * $Id: main.c 14156 2007-03-11 09:43:09Z ska $
 *
 ********************************************************************/
#include <javax/microedition/lcdui.h>
#include <javax/microedition/lcdui/game.h>
#include <java/lang.h>

static void game_loop(void)
{
  NOPH_GameCanvas_t canvas = NOPH_GameCanvas_get();
  NOPH_Graphics_t graphics = NOPH_GameCanvas_getGraphics(canvas);

  /* Select red pen, then draw a line */
  NOPH_Graphics_setColor(graphics, 255,0,0);
  NOPH_Graphics_drawLine(graphics, 10, 10, 50, 50);

  NOPH_GameCanvas_flushGraphics(canvas);
}

int main(int argc, char *argv[])
{
  game_loop();

  NOPH_Thread_sleep(2000);

  return 0;
}
