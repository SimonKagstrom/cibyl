/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      ai.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   AI-specifics
 *
 * $Id: ai.h 11530 2006-10-09 06:23:12Z ska $
 *
 ********************************************************************/
#ifndef __AI_H__
#define __AI_H__

#include "config.h"
#include "playfield.h"
#include "move.h"

int ai_minimax(playfield_t *p_playfield, item_t turn,
	       int alpha, int beta,
	       int depth, int maxdepth, move_t *p_out);
int ai_eval_playfield(playfield_t *p_playfield, item_t turn);


#endif /* !__AI_H__ */
