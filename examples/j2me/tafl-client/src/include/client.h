/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      client.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Client defs
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "config.h"

typedef struct
{
  game_t *p_game;
  int id; /* ID of this client (move game_t::id here?) */
} client_t;

#endif /* !__CLIENT_H__ */
