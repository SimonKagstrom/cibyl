/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      main.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   J2ME test app
 *
 * $Id: main.c 14049 2007-03-08 18:44:56Z ska $
 *
 ********************************************************************/
#include <javax/microedition/lcdui.h>
#include <javax/microedition/lcdui/game.h>
#include <java/lang.h>
#include <stdio.h>

static void keyPressed(int keyCode)
{
  printf("Key pressed:  %d\n", keyCode);
}

static void keyReleased(int keyCode)
{
  printf("Key released: %d\n", keyCode);
}

static void game_loop(void)
{
  NOPH_Canvas_registerKeyPressedCallback(keyPressed);
  NOPH_Canvas_registerKeyReleasedCallback(keyReleased);

  while(1)
    {
      /* Something has to be called for the events to be delivered */
      NOPH_Thread_sleep(100);
    }
}

int main(int argc, char *argv[])
{
  printf("Press keys and see the keycodes.\n");

  game_loop();

  return 0;
}
