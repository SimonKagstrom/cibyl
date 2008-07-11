/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      sokoban.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Definitions for the sokoban game
 *
 * $Id: sokoban.h 6872 2006-03-02 09:29:54Z ska $
 *
 ********************************************************************/
#ifndef __SOKOBAN_H__
#define __SOKOBAN_H__

/* Ball structure */
typedef struct
{
  int x; /* map position */
  int y;
  NOPH_Sprite_t sprite; /* The sprite of the ball */
} ball_t;

/* A structure for the player */
typedef struct
{
  int x; /* map position */
  int y;
  NOPH_Sprite_t sprite; /* The sprite of the player */
} player_t;

/* A level, see levels.h */
typedef struct
{
  int *p_level_data;
  int  w;
  int  h;
  int  player_x;
  int  player_y;
  ball_t  *p_balls; /* A pointer to the balls */
  int      n_balls; /* The number of balls */
} level_t;

/* The game */
typedef struct
{
  level_t  *p_level;
  player_t  player;
  ball_t    balls[8];
  int       taken_balls; /* How many balls do we have? */
  int   bg_x;        /* The part of the level which is shown. */
  int   bg_y;

  int   screen_w;
  int   screen_h;

  NOPH_LayerManager_t layerManager;
  NOPH_TiledLayer_t   tilemap;
  NOPH_Image_t        tileImage;
  NOPH_GameCanvas_t   canvas;
  NOPH_Graphics_t     graphics;
} game_t;


/* The tile types */
#define FLOOR   1
#define WALL    2
#define HOLE    3

#define TILE_WIDTH  16
#define TILE_HEIGHT 16

/* How long between the moves */
#define SLEEP_PERIOD 100

#endif /* !__SOKOBAN_H__ */
