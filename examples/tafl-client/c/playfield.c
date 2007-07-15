/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      playfield.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   layfield implementation for tafl game
 *
 * $Id: playfield.c 11837 2006-11-01 12:10:18Z ska $
 *
 ********************************************************************/
#include "playfield.h"
#include "utils.h"

/* Old macros... Maybe fix some day */
bool_t IS_THRONE(playfield_t *p_playfield, int x, int y)
{
  return (x == PF_WIDTH/2 && y == PF_HEIGHT/2);
}

bool_t IS_CORNER(playfield_t *p_playfield, int x, int y)
{
  return ((x == 0          && y == 0) ||
	  (x == PF_WIDTH-1 && y == 0) ||
	  (x == 0          && y == PF_HEIGHT-1) ||
	  (x == PF_WIDTH-1 && y == PF_HEIGHT-1));
}

bool_t playfield_item_is_valid(playfield_t *p_playfield, int x, int y)
{
  return !(x >= PF_WIDTH || y >= PF_HEIGHT);
}

/* These two functions should be optimized since they are called
 * basically all the time
 */
item_t playfield_get_item(playfield_t *p_playfield, int x, int y)
{
  if (playfield_item_is_valid(p_playfield, x, y))
    return p_playfield->field[y * PF_WIDTH_PWR_TWO + x];
  return PF_INVALID;
}

void playfield_set_item(playfield_t *p_playfield, item_t item, int x, int y)
{
  if (playfield_item_is_valid(p_playfield, x, y))
    p_playfield->field[y * PF_WIDTH_PWR_TWO + x] = item;
}

bool_t playfield_can_move_xy(playfield_t *p_playfield, int sx, int sy, int dx, int dy)
{
  item_t item = playfield_get_item(p_playfield, sx, sy);
  int dir_offs_x, dir_offs_y;
  dir_t dir;

  /* Must be a piece, not empty */
  if (item == PF_EMPTY || item == PF_INVALID)
    return 0;

  /* Move like a rook from the current tile */
  if ( !((sy-dy == 0 && abs(sx - dx) >= 1) ||
	 (sx-dx == 0 && abs(sy - dy) >= 1) ))
    return 0;

  /* Not allowed to step onto the corner tile unless king */
  if ( (item != PF_WHITE_KING && IS_CORNER(p_playfield, dx, dy)) ||
       IS_THRONE(p_playfield, dx, dy))
    return 0;

  dir = dx_dy_to_dir(sx, sy, dx, dy);
  dir_offs_x = dir_to_dx(dir);
  dir_offs_y = dir_to_dy(dir);

  /* Check the path from sx,sy -> dx,dy */
  while (sx != dx || sy != dy)
    {
      item_t next;

      sx = sx + dir_offs_x;
      sy = sy + dir_offs_y;
      next = playfield_get_item(p_playfield, sx, sy);
      if (next != PF_EMPTY)
	return 0;
    }

  return 1;
}

bool_t playfield_can_move(playfield_t *p_playfield, move_t *p_move)
{
  return playfield_can_move_xy(p_playfield, p_move->sx, p_move->sy, p_move->dx, p_move->dy);
}

/* Assumes that this has been checked with playfield_can_move */
void playfield_do_move_xy(playfield_t *p_playfield, int sx, int sy, int dx, int dy)
{
  item_t src;
  dir_t i;

  /* Do the actual move */
  src = playfield_get_item(p_playfield, sx, sy);
  playfield_set_item(p_playfield, src, dx, dy);
  playfield_set_item(p_playfield, PF_EMPTY, sx, sy);

  /* Check if some neighbor should be removed */
  for (i = 0; i < N_NEIGHBORS/2; i++)
    {
      int dir_dx = dir_to_dx(i);
      int dir_dy = dir_to_dy(i);
      int n_x = dx + dir_dx;
      int n_y = dy + dir_dy;
      int opp_x = dx + dir_dx * 2 ; /* n_x + dir_to_dx(i) */
      int opp_y = dy + dir_dy * 2;
      item_t neighbor = playfield_get_item(p_playfield, n_x, n_y);
      item_t neighbor_opposite = playfield_get_item(p_playfield, opp_x, opp_y);

      /* Remove neighbor piece if the far pos is a piece and of the same color as this */
      if (IS_PIECE(neighbor) &&
	  COLOR(neighbor) != COLOR(src) &&
	  (COLOR(neighbor_opposite) == COLOR(src) ||
	   IS_CORNER(p_playfield, opp_x, opp_y) ||
	   (IS_THRONE(p_playfield, opp_x, opp_y) && neighbor_opposite == PF_EMPTY)))
	{
	  playfield_set_item(p_playfield, PF_EMPTY, n_x, n_y);
	}
    }
}


int playfield_generate_possible_moves(playfield_t *p_playfield, move_t *p_moves, int sx, int sy)
{
  item_t cur = playfield_get_item(p_playfield, sx, sy);
  int out = 0;
  int i;

  for (i = 0; i < N_NEIGHBORS/2; i++)
    {
      int dist;

      /* Generate all possible moves in the current dir */
      for (dist = 1; dist < PF_WIDTH; dist++)
	{
	  int dx = sx + dir_to_dx(i)*dist;
	  int dy = sy + dir_to_dy(i)*dist;

	  /* Hit an obstacle, no need to check the rest */
	  if (!playfield_can_move_xy(p_playfield, sx, sy, dx, dy))
	    break;

	  p_moves[out] = move_new(cur, sx, sy, dx, dy);
	  out++;
	}
    }

  return out;
}


void playfield_do_move(playfield_t *p_playfield, move_t *p_move)
{
  playfield_do_move_xy(p_playfield, p_move->sx, p_move->sy, p_move->dx, p_move->dy);
}

bool_t playfield_to_string(playfield_t *p_src, char *dst, int buflen)
{
  int x,y;

  if (buflen < PF_BUFLEN)
    return FALSE;

  strncpy(dst, "PLYF", buflen);
  dst += 4;

  for (y = 0; y < PF_HEIGHT; y++)
    {
      for (x = 0; x < PF_WIDTH; x++)
	{
	  int_to_charbuf(playfield_get_item(p_src, x, y), dst, 1);
	  dst++;
	}
    }

  dst++;
  *dst = '\0';

  return TRUE;
}

bool_t playfield_from_string(playfield_t *p_dst, char *src, int buflen)
{
  int x,y;

  if (buflen < PF_BUFLEN || strncmp(src, "PLYF", buflen) != 0)
    return FALSE;

  /* The field is actually slightly larger for efficiency reasons */
  memset(p_dst, PF_INVALID, PF_WIDTH_PWR_TWO * PF_HEIGHT);

  for (y = 0; y < PF_HEIGHT; y++)
    {
      for (x = 0; x < PF_WIDTH; x++)
	{
	  item_t cur;

	  src++;
	  cur = (item_t)charbuf_to_int(src, 1);
	  if (!IS_VALID_ITEM(cur))
	    return FALSE;

	  playfield_set_item(p_dst, (item_t)cur, x, y);
	}
    }

  return TRUE;
}


#define W PF_WHITE
#define K PF_WHITE_KING
#define B PF_BLACK
#define e PF_EMPTY
#define i PF_INVALID

playfield_t playfield_initial =
{
  {
    e,e,e,B,B,B,B,B,e,e,e, i,i,i,i,i,
    e,e,e,e,e,B,e,e,e,e,e, i,i,i,i,i,
    e,e,e,e,e,e,e,e,e,e,e, i,i,i,i,i,
    B,e,e,e,e,W,e,e,e,e,B, i,i,i,i,i,
    B,e,e,e,W,W,W,e,e,e,B, i,i,i,i,i,
    B,B,e,W,W,K,W,W,e,B,B, i,i,i,i,i,
    B,e,e,e,W,W,W,e,e,e,B, i,i,i,i,i,
    B,e,e,e,e,W,e,e,e,e,B, i,i,i,i,i,
    e,e,e,e,e,e,e,e,e,e,e, i,i,i,i,i,
    e,e,e,e,e,B,e,e,e,e,e, i,i,i,i,i,
    e,e,e,B,B,B,B,B,e,e,e, i,i,i,i,i,
  },
};
#undef  W
#undef  K
#undef  B
#undef  e
#undef  i
