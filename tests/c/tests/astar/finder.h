/*********************************************************************
 *
 * Copyright (C) 2005,  Blekinge Institute of Technology
 *
 * Filename:      finder.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Finder defs
 *
 * $Id: finder.h 12265 2006-11-20 07:02:49Z ska $
 *
 ********************************************************************/
#ifndef __FINDER_H__
#define __FINDER_H__

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef abs
#define abs(x) ( (x) < 0 ? -(x) : (x) )
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @file
 * finder is a helper-library for path finding.
 */

/**
 * A direction.
 */
typedef enum
{
  NORTH = 1,    /**< North */
  EAST = 2,     /**< East */
  SOUTH = 3,    /**< South */
  WEST = 4,     /**< West */
} finder_dir_t;

/**
 * A node in the world. Every tile in the world has a node.
 */
typedef struct
{
  int x;           /**< The X-position of the node */
  int y;           /**< The Y-position of the node */
  int cost;        /**< The cost of entering this node */
  void *p_private; /**< Pointer to further private data. The user can
                        use this freely. */
} finder_node_t;

/**
 * A iterator over nodes. This is used with @a finder_neighbor_iterator_first
 * and @a finder_neighbor_iterator_next.
 */
typedef struct
{
  finder_node_t *p_node;
  finder_dir_t dir;
} finder_iterator_t;

typedef struct s_finder_path_elem
{
  finder_node_t *p_node;
  struct s_finder_path_elem *p_next;
} finder_path_elem_t;

/**
 * A path of nodes. Use @a finder_path_new to create a @a finder_path_t
 * and @a finder_path_add_first and @a finder_path_add_last to add
 * entries to the finder path.
 */
typedef struct
{
  finder_path_elem_t *p_first;
  finder_path_elem_t *p_last;
} finder_path_t;

struct s_game;
/**
 * The finder structure. This is used for all finder_-functions.
 */
typedef struct s_finder
{
  int *map;
  int map_w;
  int map_h;
  finder_node_t *p_nodes;
  finder_path_t *(*fn)(struct s_finder *p_finder, int sx, int sy, int dx, int dy);
  int *cost_vector;
  void *p_private;        /**< Pointer to further private data. This can be used freely. */
  struct s_game *p_game; /* Evil */
} finder_t;

/**
 * Initialize the finder library.
 *
 * @param p_finder the finder-object to initialize
 * @param p_tm the tilemap the finder exists in. Must be initialized
 *        before calling this function.
 * @param fn a pointer to a shortest-path function (e.g., A*), NULL
 *        selects no function.
 * @param cost_vector a vector of costs for every tile (e.g., tarmac
 *        5, dirt road 10 etc.)
 *
 * @return 0 on success, < 0 otherwise
 */
extern int finder_init(finder_t *p_finder, int *map, int map_w, int map_h,
		       finder_path_t *(*fn)(finder_t *p_finder, int sx, int sy, int dx, int dy),
		       int *cost_vector);

/**
 * Reset the finder library. This will remove all marks and set all
 * p_private members of @a finder_t to NULL
 *
 * @param p_finder the finder object
 */
void finder_reset(finder_t *p_finder);

/**
 * Set the shortest-path function pointer.
 *
 * @param p_finder the finder object
 * @param fn the pointer to the shortest-path function
 */
extern void __inline__ finder_set_fn(finder_t *p_finder, finder_path_t *(fn)(finder_t *p_finder, int sx, int sy, int dx, int dy))
{
  p_finder->fn = fn;
}


/**
 * Lookup a node on the map. Note that the coordinates are
 * world-coordinates and not tilemap coordinates (the tile coordinate
 * [1,1] is [1*TILE_W,1*TILE_H] in world-coordinates).
 *
 * @param p_finder the finder object
 * @param x the X-position, in world-coordinates, to lookup
 * @param y the Y-position, in world-coordinates, to lookup
 *
 * @return a pointer to the node at [@a x, @a y] or NULL if  [@a x, @a y]
 *         is outside the map.
 */
extern finder_node_t *finder_get_node(finder_t *p_finder, int x, int y);


/**
 * Lookup a neighbor-node to a node.
 *
 * @param p_finder the finder object
 * @param p_node the node to lookup
 * @param dir the direction to lookup in
 *
 * @return a pointer to the neighboring node or NULL if the neighbor
 *         is outside the map.
 */
extern finder_node_t *finder_neighbor_dir(finder_t *p_finder, finder_node_t *p_node, finder_dir_t dir);

/**
 * Initialize a @a finder_iterator_t to iterate over all neighbors of a node.
 *
 * @param p_finder the finder object
 * @param p_node the node to iterate at
 * @param p_it the iterator to initialize
 *
 * @return the first neighbor of the node @a p_node
 */
extern finder_node_t *finder_neighbor_iterator_first(finder_t *p_finder, finder_node_t *p_node, finder_iterator_t *p_it);

/**
 * Iterate to the next neighboring node. The iterator must be
 * initialized with @a finder_neighbor_iterator_first before calling
 * this function.
 *
 * @param p_finder the finder object
 * @param p_it the iterator
 *
 * @return the next neighbor of the node @a p_node, or NULL if there
 *         are no more neighbors
 */
extern finder_node_t *finder_neighbor_iterator_next(finder_t *p_finder, finder_iterator_t *p_it);


/**
 * Create a new @a finder_path_t. The path is allocated with malloc,
 * free it again using free.
 *
 * @return a pointer to the newly allocated @a finder_path_t, or NULL
 * if the allocation failed.
 */
extern finder_path_t *finder_path_new(void);

/**
 * Add a node to a finder path (last in the list).
 *
 * @param p_path a pointer to the @a finder_path_t
 * @param p_node the node to add
 */
extern void finder_path_add_last(finder_path_t *p_path,
				 finder_node_t *p_node);

/**
 * Add a node to a finder path (first in the list).
 *
 * @param p_path a pointer to the @a finder_path_t
 * @param p_node the node to add
 */
extern void finder_path_add_first(finder_path_t *p_path,
				  finder_node_t *p_node);


#ifdef __cplusplus
/* _Evil_ VC++-workaround, thanks Magnus! */
inline finder_dir_t operator++( finder_dir_t &rs, int )
{
  return rs = (finder_dir_t)(rs + 1);
}
}
#endif /* __cplusplus */

#endif /* !__FINDER_H__ */
