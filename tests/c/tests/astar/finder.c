/*********************************************************************
 *
 * Copyright (C) 2005,  Blekinge Institute of Technology
 *
 * Filename:      finder.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Finder implementation
 *
 * $Id: finder.c 9729 2006-09-04 13:50:10Z ska $
 *
 ********************************************************************/
#define FPS_TO_MS(x) (1000/(x))
#define MS_TO_SLEEP FPS_TO_MS(50)

#include "finder.h"

static int dir_to_dx(finder_dir_t dir)
{
  if (dir == NORTH || dir == SOUTH)
    return 0;
  if (dir == EAST)
    return 1;

  return -1;
}

static int dir_to_dy(finder_dir_t dir)
{
  if (dir == EAST || dir == WEST)
    return 0;
  if (dir == NORTH)
    return -1;

  return 1;
}

int finder_init(finder_t *p_finder, int *map, int map_w, int map_h,
		finder_path_t *(*fn)(finder_t *p_finder, int sx, int sy, int dx, int dy),
		int *cost_vector)
{
  int x,y;

  p_finder->map = map;
  p_finder->map_w = map_w;
  p_finder->map_h = map_h;
  p_finder->fn = fn;
  p_finder->cost_vector = cost_vector;

  if ( !(p_finder->p_nodes = (finder_node_t*)malloc(sizeof(finder_node_t) * (p_finder->map_w * p_finder->map_h))) )
    {
      return -1;
    }
  memset(p_finder->p_nodes, 0, sizeof(finder_node_t) * p_finder->map_w * p_finder->map_h);

  for (y = 0; y < p_finder->map_h; y++)
    {
      for (x = 0; x < p_finder->map_w; x++)
	{
	  int node = y * p_finder->map_w + x;

	  p_finder->p_nodes[ node ].x = x;
	  p_finder->p_nodes[ node ].y = y;
	  p_finder->p_nodes[ node ].cost = cost_vector[ p_finder->map[ node ] ];
	}
    }

  return 0;
}

void finder_reset(finder_t *p_finder)
{
  int i;

  for ( i = 0; i < p_finder->map_w * p_finder->map_h; i++ )
    {
      p_finder->p_nodes[i].p_private = NULL;
    }
}

finder_node_t *finder_get_node(finder_t *p_finder, int x, int y)
{
  int map_x = x;
  int map_y = y;

  if (map_x >= p_finder->map_w || map_x < 0 ||
      map_y >= p_finder->map_h || map_y < 0)
    return NULL;

  return &p_finder->p_nodes[ map_y * p_finder->map_w + map_x ];
}

finder_node_t *finder_neighbor_dir(finder_t *p_finder, finder_node_t *p_node, finder_dir_t dir)
{
  if (!p_node)
    return NULL;
  return finder_get_node(p_finder,
			 p_node->x + dir_to_dx(dir),
			 p_node->y + dir_to_dy(dir));
}


finder_node_t *finder_neighbor_iterator_first(finder_t *p_finder, finder_node_t *p_node, finder_iterator_t *p_it)
{
  p_it->p_node = p_node;
  p_it->dir = NORTH;

  return finder_neighbor_iterator_next(p_finder, p_it);
}

finder_node_t *finder_neighbor_iterator_next(finder_t *p_finder, finder_iterator_t *p_it)
{
  finder_node_t *p_node = p_it->p_node;
  finder_node_t *p_out = NULL; /* Assignment avoids warnings */

  if (p_it->dir > WEST)
    return NULL;

  for (; p_it->dir <= WEST; p_it->dir++)
    {
      if ( (p_out = finder_neighbor_dir(p_finder, p_node, p_it->dir)) )
	break;
    }
  p_it->dir++;

  return p_out;
}


finder_path_t *finder_path_new(void)
{
  finder_path_t *p_out;

  if ( !(p_out = (finder_path_t*)malloc(sizeof(finder_path_t))) )
    return NULL;

  p_out->p_first = NULL;
  p_out->p_last = NULL;

  return p_out;
}

void finder_path_add_last(finder_path_t *p_path,
			  finder_node_t *p_node)
{
  finder_path_elem_t *p_new = (finder_path_elem_t*)malloc(sizeof(finder_path_elem_t));

  p_new->p_node = p_node;
  p_new->p_next = NULL;

  /* First insertion */
  if (p_path->p_last == NULL)
    {
      p_path->p_first = p_new;
      p_path->p_last = p_new;
    }
  else
    {
      p_path->p_last->p_next = p_new;
      p_path->p_last = p_new;
    }
}

void finder_path_add_first(finder_path_t *p_path,
			   finder_node_t *p_node)
{
  finder_path_elem_t *p_new = (finder_path_elem_t*)malloc(sizeof(finder_path_elem_t));

  p_new->p_node = p_node;
  p_new->p_next = p_path->p_first;

  /* First insertion */
  if (p_path->p_first == NULL)
    {
      p_path->p_first = p_new;
      p_path->p_last = p_new;
    }
  else
    p_path->p_first = p_new;
}
