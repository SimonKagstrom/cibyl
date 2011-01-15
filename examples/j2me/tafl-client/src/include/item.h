/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      move.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Describes an item
 *
 * $Id: item.h 11478 2006-10-06 14:20:35Z ska $
 *
 ********************************************************************/
#ifndef __ITEM_H__
#define __ITEM_H__

#define PF_BLACK      0
#define PF_WHITE      1
#define PF_WHITE_KING 2
#define PF_EMPTY      3
#define PF_INVALID    4 /* Not really needed in playfield_t::field */

typedef int item_t;

#define IS_PIECE(x)    ( (x) != PF_EMPTY && (x) != PF_INVALID )
#define IS_VALID_ITEM(x) ( (x) >= PF_BLACK && (x) <= PF_INVALID )

item_t COLOR(item_t item);

#endif /* __ITEM_H__ */
