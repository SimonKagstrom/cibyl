/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      game.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Implementation of the game
 *
 * $Id: game.c 12172 2006-11-16 12:36:58Z ska $
 *
 ********************************************************************/
#include "game.h"

bool_t game_do_move(game_t *p_game, move_t *p_move)
{
  item_t src = playfield_get_item(p_game->p_playfield, p_move->sx, p_move->sy);

  if (COLOR(p_game->turn) != COLOR(src))
    return FALSE;

  if (playfield_can_move(p_game->p_playfield, p_move))
    {
      playfield_do_move(p_game->p_playfield, p_move);

      p_game->turn = (p_game->turn == PF_WHITE) ? PF_BLACK : PF_WHITE;
      return TRUE;
    }

  return FALSE;
}

int game_run_ai(game_t *p_game)
{
  move_t move;
  int out;

  if (p_game->turn != p_game->computer_color)
    return 0;

  out = ai_minimax(p_game->p_playfield, p_game->turn,
		   -PF_INF, PF_INF,
		   0, 2, &move);
  game_do_move(p_game, &move);

  return out;
}

void game_init(game_t *p_game)
{
  memset(p_game, 0, sizeof(game_t));

  p_game->p_playfield = &p_game->cur_playfield;
  memcpy(p_game->p_playfield, &playfield_initial, sizeof(playfield_t));

  p_game->computer_color = PF_WHITE;
  p_game->turn = PF_BLACK;
  p_game->id = 0;
}

bool_t game_to_string(game_t *p_game, char *dst, int buflen)
{
  if (buflen < GAME_BUFLEN)
    return FALSE;

  dst[0]  = 'G';
  int_to_charbuf(p_game->id, dst + 1, 4);

  return playfield_to_string(p_game->p_playfield, dst + GAME_BUFLEN, buflen - GAME_BUFLEN);
}

bool_t game_from_string(game_t *p_game, char *src, int buflen)
{
  if (buflen < GAME_BUFLEN || src[0] != 'G')
    return FALSE;

  p_game->id = charbuf_to_int(src + 1, 4);

  return playfield_from_string(p_game->p_playfield, src + GAME_BUFLEN, buflen - GAME_BUFLEN);
}
