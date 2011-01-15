/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      gui.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Interface defs
 *
 * $Id: gui.h 12174 2006-11-16 12:37:24Z ska $
 *
 ********************************************************************/
#ifndef __GUI_H__
#define __GUI_H__

#include <java/lang.h>
#include <javax/microedition/lcdui.h>
#include <javax/microedition/lcdui/game.h>
#include <resource-manager.h>

#include "config.h"
#include "game.h"

#define TILE_NORMAL       1
#define TILE_NORMAL_GRAY  2
#define TILE_CORNER       3
#define TILE_CORNER_GRAY  4

#define TILE_WIDTH        16
#define TILE_HEIGHT       16

#define SPRITE_BLACK      0
#define SPRITE_WHITE      1
#define SPRITE_WHITE_KING 2
#define SPRITE_CURSOR     3

#define N_SPRITES (SPRITE_CURSOR+1)

typedef enum
{
  PLAYFIELD,
  MENU,
  DEBOUNCE,
} gui_state_t;

typedef struct
{
  NOPH_ResourceManager_t  rm;

  NOPH_GameCanvas_t       canvas;
  NOPH_Graphics_t         graphics;
  NOPH_TiledLayer_t       tilemap;
  NOPH_Image_t            sprites[N_SPRITES];
  game_t                 *p_game;

  gui_state_t             state;

  int                     cursor_x;
  int                     cursor_y;

  bool_t                  is_selected;
  int                     selected_x;
  int                     selected_y;
} gui_t;

/* Initializers */
extern void gui_init(gui_t *p_gui);
extern void gui_set_game(gui_t *p_gui, game_t *p_game);
extern void gui_do(gui_t *p_gui);

#endif /* !__GUI_H__ */
