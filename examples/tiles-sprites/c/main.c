/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      main.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   J2ME test app, described on
 *                http://spel.bth.se/index.php/Cibyl:_Writing_games
 *
 * $Id: main.c 12103 2006-11-14 20:21:20Z ska $
 *
 ********************************************************************/
#include <java/lang.h>
#include <javax/microedition/lcdui.h>
#include <javax/microedition/lcdui/game.h>
#include <resource-manager.h>

/* This is the level data */
static int level0[] =
{
  5,5,1,5,5,5,5,5,5,5,5,5,5,5,5,1,5,3,4,4,5,5,2,5,
  4,5,1,4,4,5,5,1,1,1,1,1,1,1,1,1,5,3,4,4,5,5,2,5,
  4,4,1,4,4,5,5,1,4,4,5,5,2,4,5,5,5,3,3,4,5,5,2,5,
  4,5,1,1,1,1,1,1,4,4,4,5,2,4,4,5,5,3,3,4,2,2,2,5,
  5,5,1,5,5,5,5,3,3,4,4,3,2,2,4,5,5,4,3,4,4,5,2,5,
  5,5,1,5,5,3,3,3,3,5,5,3,4,2,4,5,5,5,3,4,4,5,2,5,
  5,5,1,5,5,3,3,3,3,5,5,3,3,2,5,5,5,5,3,3,4,5,2,5,
  5,5,1,5,5,3,3,3,5,5,5,5,5,2,5,5,5,5,4,3,4,5,2,5,
  4,5,1,5,3,3,3,5,5,5,5,5,1,1,1,1,1,1,5,3,4,5,2,5,
  4,4,1,1,1,1,1,1,1,1,1,1,1,5,5,3,5,1,5,3,5,5,2,5,
  4,4,1,5,5,5,5,4,4,2,5,5,5,3,3,3,5,1,5,5,5,5,2,5,
  5,5,1,5,4,5,5,4,5,2,5,5,5,3,3,3,5,1,1,1,1,1,1,1,
  5,5,1,5,5,5,5,5,5,2,5,4,5,5,3,3,5,5,5,2,5,5,4,4,
  5,5,1,5,5,5,5,5,5,2,5,5,5,5,3,3,3,5,5,2,5,4,4,4,
  5,5,1,1,1,1,1,1,1,1,5,5,5,5,3,3,3,4,5,2,4,4,4,4,
  5,5,5,5,3,3,5,5,5,1,5,5,5,5,5,5,3,4,5,2,5,4,4,4,
  5,5,5,5,3,3,3,5,5,1,5,4,5,5,5,5,5,4,5,2,5,5,4,4,
  3,5,5,5,3,3,3,5,5,1,5,4,4,5,5,5,5,5,5,2,5,5,5,5,
  3,3,5,5,5,3,5,5,5,1,5,5,5,5,5,5,5,5,5,2,5,5,5,5,
  3,3,3,5,5,5,5,5,5,1,5,5,5,5,5,2,2,2,2,2,2,2,2,2,
  3,3,3,3,5,5,4,4,5,1,1,2,2,2,2,2,3,5,5,5,5,5,5,5,
  5,5,3,3,5,5,5,4,5,5,1,5,5,5,5,3,3,3,3,5,5,5,5,5,
  5,5,5,3,3,3,5,5,5,5,1,5,5,5,5,5,3,3,3,5,5,5,5,5,
  4,5,5,5,5,5,5,4,5,5,1,5,4,5,5,5,5,5,5,5,5,5,5,5,
};

/*
 * The data structure holding the game state. Very simple for this
 * game :-)
 */
typedef struct
{
  NOPH_ResourceManager_t rm;
  NOPH_LayerManager_t layerManager;
  NOPH_TiledLayer_t   tilemap;
  NOPH_Sprite_t       bruce;
} game_t;


/* This is the main game loop */
void game_loop(game_t *p_game)
{
  NOPH_GameCanvas_t canvas = NOPH_GameCanvas_get();
  NOPH_Graphics_t graphics = NOPH_GameCanvas_getGraphics(canvas);
  int screenWidth = NOPH_GameCanvas_getWidth(canvas);
  int screenHeight = NOPH_GameCanvas_getHeight(canvas);
  int view_x = 0;
  int view_y = 0;

  while(1)
    {
      long long before = NOPH_System_currentTimeMillis();
      int keyStates = NOPH_GameCanvas_getKeyStates(canvas);
      int sleepTime;

      /* Move the viewport around according the key presses */
      if (keyStates & NOPH_GameCanvas_LEFT_PRESSED)
	view_x -= 2;
      if (keyStates & NOPH_GameCanvas_RIGHT_PRESSED)
	view_x += 2;
      if (keyStates & NOPH_GameCanvas_UP_PRESSED)
	view_y -= 2;
      if (keyStates & NOPH_GameCanvas_DOWN_PRESSED)
	view_y += 2;

      /* Update the state - move Bruce */
      NOPH_Layer_move((NOPH_Layer_t)p_game->bruce, 1, 1);

      /* Set the view window and paint the layers */
      NOPH_LayerManager_setViewWindow(p_game->layerManager,
				      view_x, view_y,
				      screenWidth,
				      screenHeight);
      NOPH_LayerManager_paint(p_game->layerManager, graphics, 0, 0);
      NOPH_GameCanvas_flushGraphics(canvas);

      /* Fix the framerate */
      sleepTime = 33 - (NOPH_System_currentTimeMillis() - before);

      if (sleepTime > 0)
	NOPH_Thread_sleep(sleepTime);
    }
}

/* Initialize the game state */
void game_init(game_t *p_game)
{
  NOPH_Image_t tileImage;
  int x,y;

  p_game->rm = NOPH_ResourceManager_getInstance();
  tileImage = NOPH_ResourceManager_getImage(p_game->rm, "/tiles.png");
  /* These functions will throw Java exceptions on failure */

  /* Create bruce */
  p_game->bruce = NOPH_Sprite_new_imageSize( NOPH_ResourceManager_getImage(p_game->rm, "/bruce.png"), 30, 24 );

  /* Create and setup the tilemap  */
  p_game->tilemap = NOPH_TiledLayer_new(24, 24,
					tileImage,
					32, 32);
  for (x = 0; x < 24; x++)
    for (y = 0; y < 24; y++)
      NOPH_TiledLayer_setCell(p_game->tilemap, x, y, level0[ y * 24 + x]);

  /* Setup the layer manager */
  p_game->layerManager = NOPH_LayerManager_new();
  NOPH_LayerManager_append(p_game->layerManager, p_game->bruce);
  NOPH_LayerManager_append(p_game->layerManager, p_game->tilemap);
}

/* Start here :-) */
int main(int argc, char *argv[])
{
  game_t game;

  game_init(&game);
  game_loop(&game);

  return 0;
}
