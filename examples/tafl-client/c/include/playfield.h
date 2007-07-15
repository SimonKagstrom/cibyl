/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      playfield.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Playfield defs
 *
 * $Id: playfield.h 11564 2006-10-10 13:50:19Z ska $
 *
 ********************************************************************/
#ifndef __PLAYFIELD_H__
#define __PLAYFIELD_H__

#include "config.h"
#include "move.h"
#include "item.h"
#include "dir.h"
#include "utils.h"

#define PF_WIDTH      11
#define PF_HEIGHT     11

#define PF_WIDTH_PWR_TWO 16

/* FIXME: Check this */
#define PF_MAX_POSSIBLE_MOVES (11+11)

#define PF_BUFLEN (PF_WIDTH * PF_HEIGHT + 1)
#define PF_TOTAL_BUFLEN (PF_BUFLEN)

#define PF_INF     30000 /* Infinity in 16 bits! */

#define N_NEIGHBORS    8

/* Scores for various things in the game */
#define SCORE_WIN      (PF_INF/2)

typedef struct
{
  item_t field[PF_WIDTH_PWR_TWO * PF_HEIGHT];
} playfield_t;

/* Global variables */
extern playfield_t playfield_initial;

/* Function prototypes */
extern bool_t IS_THRONE(playfield_t *p_playfield, int x, int y);
extern bool_t IS_CORNER(playfield_t *p_playfield, int x, int y);
extern item_t COLOR(item_t item);
extern bool_t playfield_item_is_valid(playfield_t *p_playfield, int x, int y);
extern item_t playfield_get_item(playfield_t *p_playfield, int x, int y);
extern void playfield_set_item(playfield_t *p_playfield, item_t item, int x, int y);

extern bool_t playfield_can_move_xy(playfield_t *p_playfield, int sx, int sy, int dx, int dy);
extern bool_t playfield_can_move(playfield_t *p_playfield, move_t *p_move);

/* Assumes that this has been checked with playfield_can_move */
extern void playfield_do_move_xy(playfield_t *p_playfield, int sx, int sy, int dx, int dy);
extern void playfield_do_move(playfield_t *p_playfield, move_t *p_move);

extern bool_t playfield_to_string(playfield_t *p_src, char *dst, int buflen);
extern bool_t playfield_from_string(playfield_t *p_dst, char *src, int buflen);

extern int playfield_generate_possible_moves(playfield_t *p_playfield, move_t *p_moves, int sx, int sy);

#endif /* !__PLAYFIELD_H__ */
