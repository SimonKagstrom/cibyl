/*********************************************************************
 *
 * Copyright (C) 2004-2006,  Blekinge Institute of Technology
 *
 * Filename:      main.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Sokoban game (Cibyl).
 *
 *                See http://www.cs.ualberta.ca/~games/Sokoban/ for
 *                information about the game of Sokoban.
 *
 *                This is a port of the same game as implemented for
 *                Mophun. This is also why some things look strange
 *                (e.g., the game_goto_level function) or are wrongly
 *                implemented.
 *
 * $Id: main.c 12170 2006-11-16 12:36:23Z ska $
 *
 ********************************************************************/
#include <java/lang.h>
#include <javax/microedition/lcdui.h>
#include <javax/microedition/lcdui/game.h>
#include <resource-manager.h>
#include <string.h>

#include "sokoban.h"
#include "levels.h"

NOPH_ResourceManager_t rm;

/* --- Function prototypes --- */
int wait_for_keypress(game_t *p_game);
void game_goto_level(game_t *p_game, level_t *p_level);
void game_draw(game_t *p_game);
int ball_move(game_t *p_game, ball_t *p_ball, int dx, int dy);
void ball_draw(game_t *p_game, ball_t *p_ball);
int player_move(game_t *p_game, player_t *p_player, int dx, int dy);
void player_draw(game_t *p_game, player_t *p_player);


/* --- Some helpers --- */

/* Wait until some key is pressed */
int wait_for_keypress(game_t *p_game)
{
  int out;

  while ( (out = NOPH_GameCanvas_getKeyStates(p_game->canvas)) == 0);

  return out;
}


/* --- Game functionality --- */
/* Init the game */
void game_init(game_t *p_game)
{
  rm = NOPH_ResourceManager_getInstance();

  /* Initialise the graphics */
  p_game->canvas = NOPH_GameCanvas_get();
  p_game->graphics = NOPH_GameCanvas_getGraphics(p_game->canvas);
  p_game->tileImage = NOPH_ResourceManager_getImage(rm, "/tiles.png");

  /* Get the size of the screen */
  p_game->screen_w = NOPH_GameCanvas_getWidth(p_game->canvas);
  p_game->screen_h = NOPH_GameCanvas_getHeight(p_game->canvas);

  /* Init the first level */
  game_goto_level(p_game, &levels[0]);
}

/* Goto a game level */
void game_goto_level(game_t *p_game, level_t *p_level)
{
  int i, x, y;

  /* This is actually a memory leak, but for now we don't care */
  p_game->layerManager = NOPH_LayerManager_new();

  p_game->player.sprite = NOPH_Sprite_new( NOPH_ResourceManager_getImage(rm, "/player.png"));
  NOPH_LayerManager_append(p_game->layerManager, p_game->player.sprite);
  NOPH_LayerManager_setViewWindow(p_game->layerManager, 0, 0, p_game->screen_w, p_game->screen_h);

  p_game->tilemap = NOPH_TiledLayer_new(p_level->w,
					p_level->h,
					p_game->tileImage,
					TILE_WIDTH, TILE_HEIGHT);
  for (x = 0; x < p_level->w; x++)
    for (y = 0; y < p_level->h; y++)
      NOPH_TiledLayer_setCell(p_game->tilemap, x, y, p_level->p_level_data[ y * p_level->w + x]);

  /* Place the player */
  p_game->player.x = p_level->player_x;
  p_game->player.y = p_level->player_y;

  /* Init the game structure and copy the balls from the level */
  p_game->taken_balls = 0;
  p_game->p_level = p_level;
  for (i=0; i<p_level->n_balls; i++)
    {
      memcpy(&p_game->balls[i], &p_level->p_balls[i], sizeof(ball_t));
      p_game->balls[i].sprite = NOPH_Sprite_new( NOPH_ResourceManager_getImage(rm, "/ball.png") );

      NOPH_LayerManager_append(p_game->layerManager, p_game->balls[i].sprite);
    }

  NOPH_LayerManager_append(p_game->layerManager, p_game->tilemap);

  /* TRICK: move the player dx=0, dy=0 to center the map around the player */
  player_move(p_game, &p_game->player, 0, 0);
}

/* Draw the entire game */
void game_draw(game_t *p_game)
{
  level_t *p_level = p_game->p_level;
  int i;

  /* Place the background */
  NOPH_Layer_setPosition(p_game->tilemap, -p_game->bg_x*TILE_HEIGHT, -p_game->bg_y*TILE_HEIGHT);

  /* Place the sprites on the screen (or off it) */
  for (i=0; i<p_level->n_balls; i++)
    ball_draw(p_game, &p_game->balls[i]);
  player_draw(p_game, &p_game->player);

  /* Update sprites and the background */
  NOPH_LayerManager_paint(p_game->layerManager, p_game->graphics, 0, 0);
}

/* Move a ball, returns 1 if the move is illegal */
int ball_move(game_t *p_game, ball_t *p_ball, int dx, int dy)
{
  int new_x = p_ball->x + dx;
  int new_y = p_ball->y + dy;
  level_t *p_level = p_game->p_level;
  int i;

  /* We cannot move two balls at a time */
  for (i=0; i<p_level->n_balls; i++)
    {
      ball_t *p_curr_ball = &p_game->balls[i];

      /* Skip ourselves */
      if (p_ball == p_curr_ball)
	continue;
      if (p_curr_ball->x == new_x &&
	  p_curr_ball->y == new_y)
	return 1; /* Nope, cannot move two balls */
    }

  switch (p_level->p_level_data[new_y*p_level->w + new_x])
    {
    case WALL: /* Wall, don't move */
      return 1;
    case HOLE: /* Hole, one more ball taken (unless it was already on a hole) */
      if (p_level->p_level_data[p_ball->y*p_level->w + p_ball->x] != HOLE)
	p_game->taken_balls++;
      break;
    default:
      if (p_level->p_level_data[p_ball->y*p_level->w + p_ball->x] == HOLE)
	p_game->taken_balls--; /* Moved OFF a hole, one less taken! */
    }
  p_ball->x = new_x;
  p_ball->y = new_y;

  return 0;
}

/* Draw a ball */
void ball_draw(game_t *p_game, ball_t *p_ball)
{
  NOPH_Layer_setPosition(p_ball->sprite,  (p_ball->x-p_game->bg_x)*TILE_WIDTH, (p_ball->y-p_game->bg_y)*TILE_HEIGHT);
}

/* Update the location of the sprite */
int player_move(game_t *p_game, player_t *p_player, int dx, int dy)
{
  int new_x = p_player->x + dx; /* Where the player tries to move */
  int new_y = p_player->y + dy;
  level_t *p_level = p_game->p_level;
  int i;

  /* Push a ball? */
  for (i=0; i<p_level->n_balls; i++)
    {
      if (p_game->balls[i].x == new_x &&
	  p_game->balls[i].y == new_y)
	{
	  /* We are moving towards the ball - try to push it! */
	  if (ball_move(p_game, &p_game->balls[i], dx, dy))
	    return 1; /* Nope, not possible... */
	}
    }

  /* Stop the player if she hits the walls */
  switch (p_level->p_level_data[new_y*p_level->w + new_x])
    {
    case WALL: /* Don't move through walls! */
      break;
    default:
      p_player->x = new_x; /* Just walk otherwise */
      p_player->y = new_y;
    }

  /* Center the view around the player */
  p_game->bg_x = p_player->x - (p_game->screen_w / TILE_WIDTH) / 2;
  if (p_game->bg_x < 0)
    p_game->bg_x = 0; /* Stop at (0,?) */
  else if (p_game->bg_x > p_level->w - p_game->screen_w/TILE_WIDTH)
    p_game->bg_x = p_level->w - p_game->screen_w/TILE_WIDTH; /* Stop at (screen_w, ?)*/

  p_game->bg_y = p_player->y - (p_game->screen_h / TILE_HEIGHT) / 2;
  if (p_game->bg_y < 0)
    p_game->bg_y = 0;
  else if (p_game->bg_y > p_level->h - p_game->screen_h/TILE_HEIGHT)
    p_game->bg_y = p_level->h - p_game->screen_h/TILE_HEIGHT;

  return 0;
}

/* Draw the player sprite */
void player_draw(game_t *p_game, player_t *p_player)
{
  /* bg_x, bg_y is the background offset */
  NOPH_Layer_setPosition(p_player->sprite, (p_player->x-p_game->bg_x)*TILE_HEIGHT, (p_player->y-p_game->bg_y)*TILE_HEIGHT);
}


int main(int argc, char *argv[])
{
  int should_exit = 0;
  game_t game;
  game_t *p_game = &game;
  int curr_level = 0;

  game_init(p_game);

  /* Draw everything */
  game_draw(p_game);
  NOPH_GameCanvas_flushGraphics(p_game->canvas);

  /* The main game loop */
  while(!should_exit)
    {
      long long pre_ticks, post_ticks;
      int keys;

      /* Wait for a keypress */
      keys = wait_for_keypress(&game);

      pre_ticks = NOPH_System_currentTimeMillis(); /* Get the current ms ticks */

      /* Move the player around */
      if (keys & NOPH_GameCanvas_LEFT_PRESSED)
	player_move(&game, &game.player, -1, 0);
      else if (keys & NOPH_GameCanvas_RIGHT_PRESSED)
	player_move(&game, &game.player, 1, 0);
      else if (keys & NOPH_GameCanvas_UP_PRESSED)
	player_move(&game, &game.player, 0, -1);
      else if (keys & NOPH_GameCanvas_DOWN_PRESSED)
	player_move(&game, &game.player, 0, 1);
      else if (keys & NOPH_GameCanvas_FIRE_PRESSED)
	should_exit = 1;

      /* Draw everything */
      game_draw(&game);
      NOPH_GameCanvas_flushGraphics(p_game->canvas);

      /* If we have taken all balls - goto next level! */
      if (game.taken_balls >= game.p_level->n_balls)
	{
	  if (++curr_level < N_LEVELS)
	    game_goto_level(&game, &levels[curr_level]);
	  else
	    break; /* This was the last level! */
	}

      post_ticks = NOPH_System_currentTimeMillis(); /* Get the current ms ticks */

      /* Every loop iteration should take about SLEEP_PERIOD, see to that */
      if ((post_ticks - pre_ticks) < SLEEP_PERIOD)
	NOPH_Thread_sleep( SLEEP_PERIOD - (post_ticks-pre_ticks) );
    }

  return 0;
}
