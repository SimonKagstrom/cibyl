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
#include <s9.h>
#include <stdio.h>
#include <command_mgr.h>

#include "s9_input.h"

static s9_t s9;

static void keyPressed(int keyCode)
{
  s9_input_handle_keypress(keyCode);
}

static void keyReleased(int keyCode)
{
}

static void game_loop(void)
{
  NOPH_GameCanvas_t canvas = NOPH_GameCanvas_get();
  NOPH_Graphics_t graphics = NOPH_GameCanvas_getGraphics(canvas);

  NOPH_Canvas_registerKeyPressedCallback(keyPressed);
  NOPH_Canvas_registerKeyReleasedCallback(keyReleased);

  NOPH_Graphics_drawString(graphics,
                           "Type in s9 words (e.g., look closet)", 0, 0, NOPH_Graphics_TOP | NOPH_Graphics_LEFT);

  while(1)
    {
      NOPH_Thread_sleep(20);
      NOPH_Graphics_setColor_int(graphics, 0xffffffff);
      NOPH_Graphics_fillRect(graphics, 0, 16, 20, 256);
      NOPH_Graphics_setColor_int(graphics, 0);
      s9_input_draw(graphics, 0, 16);
      NOPH_GameCanvas_flushGraphics(canvas);
    }
}

const char *words[] =
{
  "move",
  "note",
  "take",
  "look",
  "closet",
  "I",
  NULL
};

static void save_game_callback(void *arg)
{
  printf("sg: %d\n", (int)arg);
}

static void exit_callback(void *arg)
{
  printf("exit: %d\n", (int)arg);
}

int main(int argc, char *argv[])
{
  int i;
  NOPH_CommandMgr_t cmd_mgr;

  cmd_mgr = NOPH_CommandMgr_getInstance();

  s9_init(&s9, 10);
  s9_input_init(&s9);

  NOPH_CommandMgr_addCommand(cmd_mgr, NOPH_Command_ITEM, "Save game", save_game_callback, NULL);
  NOPH_CommandMgr_addCommand(cmd_mgr, NOPH_Command_ITEM, "Sierra menu", save_game_callback, NULL);
  NOPH_CommandMgr_addCommand(cmd_mgr, NOPH_Command_BACK, "Exit", exit_callback, NULL);

  for ( i = 0;
        words[i] != NULL;
        i++ )
    {
      printf("Adding %s\n", words[i]);
      s9_add_word(&s9, words[i]);
    }

  game_loop();

  NOPH_Thread_sleep(2000);

  return 0;
}
