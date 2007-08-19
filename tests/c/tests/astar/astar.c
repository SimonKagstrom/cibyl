/*********************************************************************
 *
 * Copyright (C) 2005,  Blekinge Institute of Technology
 *
 * Filename:      pf.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Path finding implementation (A*)
 *
 * $Id: astar.c 12265 2006-11-20 07:02:49Z ska $
 *
 ********************************************************************/
#include <stdio.h>
#include <stdlib.h>

#define printf(x,...)

#include "finder.h"
#include "prio_queue.h"

typedef struct
{
  void *tree_root;
  prio_queue_t queue;
} as_t;

typedef struct
{
  int g;           /**< The 'g'-cost of a node */
  int f;           /**< The 'f'-cost of a node */
  int in_closed;
  int in_open;
  finder_node_t *p_parent;  /**< Pointer to the path to this node */
} as_node_t;

static as_node_t nodes[10000];
static as_node_t *cur = &nodes[0];

static as_node_t *allocate_node(finder_node_t *p_node)
{
  as_node_t *p_out;

  if (p_node->p_private)
    return (as_node_t*)p_node->p_private;

  p_out = cur;
  cur++;
#if 0
  if ( !(p_out = (as_node_t *)malloc(sizeof(as_node_t))) )
    exit(1);
#endif
  memset(p_out, 0, sizeof(as_node_t));
  p_node->p_private = (void*)p_out;

  return p_out;
}

int compare(const void *pa, const void *pb)
{
  as_node_t *p_a = ((finder_node_t*)pa)->p_private;
  as_node_t *p_b = ((finder_node_t*)pb)->p_private;

  if (p_a->f < p_b->f)
    return -1;
  if (p_a->f > p_b->f)
    return 1;

  return 0;
}

/* Manhattan dist */
static int heuristic(finder_t *p_finder, finder_node_t *p_a, finder_node_t *p_b)
{
  return abs( p_a->x - p_b->x ) + abs( p_a->y - p_b->y );
}


static void put_in_closed(finder_t *p_finder, finder_node_t *p_node)
{
  ((as_node_t*)p_node->p_private)->in_closed = 1;
}

static void remove_from_closed(finder_t *p_finder, finder_node_t *p_node)
{
  ((as_node_t*)p_node->p_private)->in_closed = 0;
}

static int is_in_closed(finder_t *p_finder, finder_node_t *p_node)
{
  return ((as_node_t*)p_node->p_private)->in_closed;
}


static void put_in_open(finder_t *p_finder, finder_node_t *p_node)
{
  as_t *p = (as_t*)p_finder->p_private;

  ((as_node_t*)p_node->p_private)->in_open = 1;

  prio_queue_insert( &p->queue, p_node, compare );
}

static int is_in_open(finder_t *p_finder, finder_node_t *p_node)
{
  return ((as_node_t*)p_node->p_private)->in_open;
}

static finder_node_t *get_from_open(finder_t *p_finder)
{
  as_t *p = (as_t*)p_finder->p_private;
  finder_node_t *p_first = prio_queue_get_first(&p->queue);

  if (!p_first)
    return NULL;
  ((as_node_t*)p_first->p_private)->in_open = 0;

  return p_first;
}

static finder_path_t *construct_path(finder_t *p_finder, finder_node_t *p_node)
{
  finder_path_t *p_out = finder_path_new();
  finder_node_t *p;

  if (!p_out)
    return NULL;

  for ( p = p_node;
	p;
	p = ((as_node_t*)p->p_private)->p_parent )
    {
      finder_path_add_first(p_out, p);
    }

  return p_out;
}

int checks;

finder_path_t *a_star_implementation(finder_t *p_finder, int sx, int sy, int dx, int dy)
{
  finder_node_t *p_start = finder_get_node(p_finder, sx, sy);
  finder_node_t *p_end = finder_get_node(p_finder, dx, dy);
  finder_node_t *p_node;
  as_node_t *p;

  p = allocate_node(p_start);
  p->g = 0;
  p->f = p->g + heuristic(p_finder, p_start, p_end); /* g+h */
  p->p_parent = NULL;

  put_in_open(p_finder, p_start);

  /* While there are nodes in the Open set */
  while ( (p_node = get_from_open(p_finder)) )
    {
      finder_iterator_t it;
      finder_node_t *p_succ;

      /* We found a path! */
      if ( p_node == p_end )
	return construct_path(p_finder, p_node);

      /* Iterate over UP, DOWN, LEFT and RIGHT */
      for ( p_succ = finder_neighbor_iterator_first(p_finder, p_node, &it);
	    p_succ;
	    p_succ = finder_neighbor_iterator_next(p_finder, &it) )
	{
	  as_node_t *p_succ_priv = allocate_node(p_succ);
	  as_node_t *p_node_priv = (as_node_t*)allocate_node(p_node);
	  int newg;

	  newg = p_node_priv->g + p_succ->cost;

	  checks++;

	  if ( (is_in_open(p_finder, p_succ) || is_in_closed(p_finder, p_succ)) &&
	       p_succ_priv->g <= newg)
	    continue;  /* We have a better value or this is not valid */

	  p_succ_priv->p_parent = p_node;
	  p_succ_priv->g = newg;
	  p_succ_priv->f = p_succ_priv->g + heuristic(p_finder, p_succ, p_end); /* g+h */

	  printf("Checking (%d,%d - %d): %.2f,%.2f,%.2f\n", p_succ->x, p_succ->y, p_succ->cost,
		 p_succ_priv->g, p_succ_priv->f, heuristic(p_finder, p_succ, p_end) );

	  if (is_in_closed(p_finder, p_succ))
	    remove_from_closed(p_finder, p_succ);

	  if (!is_in_open(p_finder, p_succ))
	    put_in_open(p_finder, p_succ);
	}
      put_in_closed(p_finder, p_node);
    }

  /* There was no path */
  return NULL;
}

void pf_init(finder_t *p_finder)
{
  as_t *p_as = (as_t*)malloc(sizeof(as_t));

  if (!p_as)
    exit(1);

  finder_set_fn(p_finder, a_star_implementation);
  p_finder->p_private = (void*)p_as;
  p_as->tree_root = NULL;

  prio_queue_init(&p_as->queue);
}

void pf_fini(finder_t *p_finder)
{
  as_t *p_as = (as_t*)p_finder->p_private;

  printf("pf_fini\n");
  /* Pop all entries from the queue */
  while (prio_queue_get_first(&p_as->queue));
}
