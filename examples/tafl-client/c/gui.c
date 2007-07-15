/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      gui.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Interface to the tafl game
 *
 * $Id: gui.c 11609 2006-10-12 10:24:49Z ska $
 *
 ********************************************************************/
#include "gui.h"

void gui_add_captured_piece(gui_t *p_gui, item_t item)
{
}

void gui_draw_one_pos(gui_t *p_gui, int x, int y)
{
  item_t item = playfield_get_item(p_gui->p_game->p_playfield, x, y);
  NOPH_Image_t img = 0;

  switch(item)
    {
    case PF_BLACK:
      img = p_gui->sprites[SPRITE_BLACK];
      break;
    case PF_WHITE:
      img = p_gui->sprites[SPRITE_WHITE];
      break;
    case PF_WHITE_KING:
      img = p_gui->sprites[SPRITE_WHITE_KING];
      break;
    default:
      break;
    }
  if (img)
    NOPH_Graphics_drawImage(p_gui->graphics, img, x * TILE_WIDTH, y * TILE_HEIGHT,
			    NOPH_Graphics_TOP | NOPH_Graphics_LEFT);
  if (p_gui->cursor_x == x &&
      p_gui->cursor_y == y)
    NOPH_Graphics_drawImage(p_gui->graphics, p_gui->sprites[SPRITE_CURSOR],
			    x * TILE_WIDTH - 2, y * TILE_HEIGHT - 2,
			    NOPH_Graphics_TOP | NOPH_Graphics_LEFT);
}

static int gui_keys_to_dx(int keys)
{
  return (keys & NOPH_GameCanvas_LEFT_PRESSED) ? -1 : (keys & NOPH_GameCanvas_RIGHT_PRESSED) ? 1 : 0;
}

static int gui_keys_to_dy(int keys)
{
  return (keys & NOPH_GameCanvas_UP_PRESSED) ? -1 : (keys & NOPH_GameCanvas_DOWN_PRESSED) ? 1 : 0;
}

void gui_set_field_xy(gui_t *p_gui, int x, int y, bool_t gray)
{
  playfield_t *p = p_gui->p_game->p_playfield;
  int tile = (IS_CORNER(p, x, y) || IS_THRONE(p, x, y)) ? TILE_CORNER : TILE_NORMAL;

  if (gray)
    tile++;

  NOPH_TiledLayer_setCell(p_gui->tilemap, x, y, tile);
}

void gui_clear_field(gui_t *p_gui, bool_t gray)
{
  int x,y;

  for (y = 0; y < PF_HEIGHT; y++)
    {
      for (x = 0; x < PF_WIDTH; x++)
	gui_set_field_xy(p_gui, x, y, gray);
    }
}

void gui_highlight_selected(gui_t *p_gui, move_t *possible_moves, int n_possible_moves)
{
  int i;

  /* Clear to gray */
  gui_clear_field(p_gui, TRUE);
  for (i = 0; i < n_possible_moves; i++)
    gui_set_field_xy(p_gui, possible_moves[i].dx, possible_moves[i].dy, FALSE);
}

void gui_draw_playfield(gui_t *p_gui)
{
  int x,y;

  NOPH_TiledLayer_paint(p_gui->tilemap, p_gui->graphics);

  for (y = 0; y < PF_HEIGHT; y++)
    {
      for (x = 0; x < PF_WIDTH; x++)
	gui_draw_one_pos(p_gui, x, y);
    }
}


void gui_handle_input_playfield(gui_t *p_gui, int keys)
{
  int new_x = p_gui->cursor_x + gui_keys_to_dx(keys);
  int new_y = p_gui->cursor_y + gui_keys_to_dy(keys);

  if (new_x >= 0 && new_x < PF_WIDTH)
    p_gui->cursor_x = new_x;
  if (new_y >= 0 && new_y < PF_HEIGHT)
    p_gui->cursor_y = new_y;

  if (keys & NOPH_GameCanvas_FIRE_PRESSED)
    {
      item_t cur = playfield_get_item(p_gui->p_game->p_playfield,
				      p_gui->cursor_x, p_gui->cursor_y);

      /* Get the possible moves from x,y if this is our turn */
      if (!p_gui->is_selected &&
	  COLOR(cur) == COLOR(p_gui->p_game->turn))
	{
	  move_t possible_moves[PF_MAX_POSSIBLE_MOVES];
	  int n_possible_moves = playfield_generate_possible_moves(p_gui->p_game->p_playfield, possible_moves,
								   p_gui->cursor_x, p_gui->cursor_y);

	  gui_highlight_selected(p_gui, possible_moves, n_possible_moves);

	  p_gui->is_selected = TRUE;
	  p_gui->selected_x = p_gui->cursor_x;
	  p_gui->selected_y = p_gui->cursor_y;
	}
      else if (p_gui->is_selected)
	{
	  if (cur == PF_EMPTY)
	    {
	      move_t move = move_new(p_gui->p_game->turn,
				     p_gui->selected_x, p_gui->selected_y,
				     p_gui->cursor_x, p_gui->cursor_y);

	      game_do_move(p_gui->p_game, &move);
	    }
	  gui_clear_field(p_gui, FALSE);
	  p_gui->is_selected = FALSE;
	}
    }
}

void gui_do(gui_t *p_gui)
{
  while (1)
    {
      long long before = NOPH_System_currentTimeMillis();
      int keys = NOPH_GameCanvas_getKeyStates(p_gui->canvas);
      int sleep_time;

      if (p_gui->p_game->turn == p_gui->p_game->computer_color)
	game_run_ai(p_gui->p_game);

      gui_handle_input_playfield(p_gui, keys);
      gui_draw_playfield(p_gui);

      NOPH_GameCanvas_flushGraphics(p_gui->canvas);

      sleep_time = 150 - (NOPH_System_currentTimeMillis() - before);
      if (sleep_time > 0)
	NOPH_Thread_sleep(sleep_time);
    }
}

void gui_set_game(gui_t *p_gui, game_t *p_game)
{
  p_gui->p_game = p_game;
}

void gui_init(gui_t *p_gui)
{
  NOPH_Image_t tiles;

  p_gui->p_game = NULL;

  p_gui->cursor_x = PF_WIDTH/2;
  p_gui->cursor_y = PF_HEIGHT/2;

  p_gui->is_selected = FALSE;

  p_gui->rm = NOPH_ResourceManager_getInstance();
  p_gui->canvas = NOPH_GameCanvas_get();
  p_gui->graphics = NOPH_GameCanvas_getGraphics(p_gui->canvas);

  NOPH_Canvas_setFullScreenMode(p_gui->canvas, TRUE);

  /* Load images */
  FATAL_ON_EXCEPTION(tiles = NOPH_ResourceManager_getImage(p_gui->rm, "/tiles.png"));
  FATAL_ON_EXCEPTION(p_gui->sprites[SPRITE_BLACK] = NOPH_ResourceManager_getImage(p_gui->rm, "/black.png"));
  FATAL_ON_EXCEPTION(p_gui->sprites[SPRITE_WHITE] = NOPH_ResourceManager_getImage(p_gui->rm, "/white.png"));
  FATAL_ON_EXCEPTION(p_gui->sprites[SPRITE_WHITE_KING] = NOPH_ResourceManager_getImage(p_gui->rm, "/white-king.png"));
  FATAL_ON_EXCEPTION(p_gui->sprites[SPRITE_CURSOR] = NOPH_ResourceManager_getImage(p_gui->rm, "/cursor.png"));

  /* Cretae the tilemap */
  p_gui->tilemap = NOPH_TiledLayer_new(PF_WIDTH, PF_HEIGHT,
				       tiles,
				       TILE_WIDTH, TILE_HEIGHT);
  gui_clear_field(p_gui, FALSE);
}
