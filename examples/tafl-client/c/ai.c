/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      ai.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Tafl game AI
 *
 * $Id: ai.c 11530 2006-10-09 06:23:12Z ska $
 *
 ********************************************************************/
#include "ai.h"

static int ai_eval_one_pos_black(playfield_t *p_playfield,
				 item_t turn, item_t cur, int x, int y)
{
  int ret = 5; /* More important than the neighbors */
  dir_t i;

  for (i = 0; i < N_NEIGHBORS/2; i++)
    {
      item_t neighbor = playfield_get_item(p_playfield, x + dir_to_dx(i), y + dir_to_dy(i));

      /* Good to have a friend near */
      if (COLOR(neighbor) == COLOR(cur))
	ret++;
      else if (IS_PIECE(neighbor))
	{
	  if (turn == PF_WHITE)
	    ret--; /* ... But bad to have the enemy up close */
	  else
	    ret++;
	}
    }

  return ret;
}

static int ai_eval_one_pos_white_king(playfield_t *p_playfield,
				      item_t turn, item_t cur, int x, int y)
{
  int ret = -SCORE_WIN; /* The king is alive! */
  dir_t i;

  if (IS_CORNER(p_playfield, x, y))
    return -SCORE_WIN*2;
  /* Good to be close to the corners */
  ret += abs(x - PF_WIDTH/2) + abs(y - PF_HEIGHT/2);

  if ((x == 0 || x == PF_WIDTH-1) ||
      (y == 0 || y == PF_HEIGHT-1))
    ret -= 30; /* On a line towards the corners */
  for (i = 0; i < N_NEIGHBORS/2; i++)
    {
      item_t neighbor = playfield_get_item(p_playfield, x + dir_to_dx(i), y + dir_to_dy(i));

      if (IS_PIECE(neighbor))
	{
	  ret++;
	  if (COLOR(neighbor) == PF_BLACK)
	    ret++;
	}
    }

  return ret;
}

static int ai_eval_one_pos_white(playfield_t *p_playfield,
				 item_t turn, item_t cur, int x, int y)
{
  int ret = -5; /* The value of one piece */
  dir_t i;

  for (i = 0; i < N_NEIGHBORS/2; i++)
    {
      item_t neighbor = playfield_get_item(p_playfield, x + dir_to_dx(i), y + dir_to_dy(i));

      /* White doesn't need to stay close to friends */
      if (COLOR(neighbor) == PF_BLACK)
	ret++; /* ... But bad to have the enemy up close */
    }

  return ret;
}

/* Black is positive, white negative */
int ai_eval_playfield(playfield_t *p_playfield, item_t turn)
{
  int x,y;
  int ret = SCORE_WIN; /* Assume king is taken */

  for (y = 0; y < PF_HEIGHT; y++)
    {
      for (x = 0; x < PF_WIDTH; x++)
	{
	  item_t cur = playfield_get_item(p_playfield, x, y);

	  switch(cur)
	    {
	    case PF_BLACK:
	      ret += ai_eval_one_pos_black(p_playfield, turn, cur, x, y);
	      break;
	    case PF_WHITE:
	      ret += ai_eval_one_pos_white(p_playfield, turn, cur, x, y);
	      break;
	    case PF_WHITE_KING:
	      ret += ai_eval_one_pos_white_king(p_playfield, turn, cur, x, y);
	      break;
	    default:
	      break;
	    }
	}
    }

  return ret;
}

/* Implementation of the minimax algorithm. */
int ai_minimax(playfield_t *p_playfield, item_t turn,
	       int alpha, int beta,
	       int depth, int maxdepth, move_t *p_out)
{
  item_t next_turn;
  int x,y;
  int best_eval;

  /* End the recursion */
  if (depth >= maxdepth)
    return ai_eval_playfield(p_playfield, turn);
  best_eval = (turn == PF_BLACK) ? -PF_INF : PF_INF;
  next_turn = (turn == PF_WHITE) ? PF_BLACK : PF_WHITE;

  for (y = 0; y < PF_HEIGHT; y++)
    {
      for (x = 0; x < PF_WIDTH; x++)
	{
	  item_t cur = playfield_get_item(p_playfield, x, y);
	  dir_t i;

	  if (COLOR(cur) != turn)
	    continue;

	  for (i = 0; i < N_NEIGHBORS/2; i++)
	    {
	      move_t move;
	      int dist;

	      /* Generate all possible moves in the current dir */
	      for (dist = 1; dist < PF_WIDTH; dist++)
		{
		  playfield_t next_playfield;
		  int eval;
		  int dx = x + dir_to_dx(i)*dist;
		  int dy = y + dir_to_dy(i)*dist;

		  /* Hit an obstacle, no need to check the rest */
		  if (!playfield_can_move_xy(p_playfield, x, y, dx, dy))
		    break;

		  /* Execute move on a new playfield */
		  memcpy(&next_playfield, p_playfield, sizeof(playfield_t));
		  playfield_do_move_xy(&next_playfield, x, y, dx, dy);

		  /* Evaluate recursively */
		  eval = ai_minimax(&next_playfield, next_turn,
				    alpha, beta,
				    depth + 1, maxdepth, &move);
		  if ( (turn == PF_BLACK && eval >= best_eval) ||
		       (turn == PF_WHITE && eval <= best_eval) )
		    {
		      if (eval == best_eval &&
			  (rand() & 4) != 0)
			continue;

		      *p_out = move_new(turn, x, y, dx, dy);
		      best_eval = eval;
		    }
		  if (turn == PF_WHITE)
		    {
		      beta = min(beta, eval);
		      if (beta <= alpha)
			return beta;
		      best_eval = beta;
		    }
		  else
		    {
		      alpha = max(alpha, eval);
		      if (beta <= alpha)
			return alpha;
		      best_eval = alpha;
		    }
		}
	    }
	}
    }

  return best_eval;
}
