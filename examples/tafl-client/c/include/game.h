/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      game.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Game defs
 *
 * $Id: game.h 11604 2006-10-12 10:18:56Z ska $
 *
 ********************************************************************/
#ifndef __GAME_H__
#define __GAME_H__

#include "config.h"
#include "playfield.h"
#include "move.h"
#include "ai.h"

/* game id, starting color */
#define GAME_BUFLEN (1+4)
#define GAME_TOTAL_BUFLEN (GAME_BUFLEN + PF_TOTAL_BUFLEN)

typedef struct s_game
{
  playfield_t *p_playfield;
  item_t computer_color;          /* PF_INVALID for two-player games */

  item_t turn;

  int id;     /* The id of this game, 4 significant digits */

  playfield_t cur_playfield; /* p_playfield will point here */
} game_t;

extern void game_init(game_t *p_game);
extern bool_t game_do_move(game_t *p_game, move_t *p_move);
extern int game_run_ai(game_t *p_game);

extern bool_t game_to_string(game_t *p_game, char *dst, int buflen);
extern bool_t game_from_string(game_t *p_game, char *dst, int buflen);


#endif /* !__GAME_H__ */
