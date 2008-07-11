/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      main.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Tafl game
 *
 * $Id: main.c 11523 2006-10-08 14:26:25Z ska $
 *
 ********************************************************************/
#include <cibyl.h>

#include "gui.h"
#include "game.h"

int main(int argc, char *argv[])
{
  game_t game;
  gui_t gui;

  srand(NOPH_System_currentTimeMillis());

  game_init(&game);
  gui_init(&gui);
  gui_set_game(&gui, &game);

  /* Game loop */
  gui_do(&gui);

  return 0;
}
