/*********************************************************************
 *
 * Copyright (C) 2005,  Blekinge Institute of Technology
 *
 * Filename:      prio_queue.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Priority queue implementation
 *
 * $Id: prio_queue.c 12265 2006-11-20 07:02:49Z ska $
 *
 ********************************************************************/
#include <stdlib.h>

#include "prio_queue.h"

void prio_queue_init(prio_queue_t *p_q)
{
  p_q->p_first = NULL;
}

extern int compare(const void *pa, const void *pb);
/* Insert p_node into p_q, sorted by f */
void prio_queue_insert(prio_queue_t *p_q, void *p_data, int (*pare)(const void *pa, const void *pb) )
{
  prio_queue_elem_t *p = p_q->p_first;
  prio_queue_elem_t *p_last = NULL;
  prio_queue_elem_t *p_new = (prio_queue_elem_t*)malloc(sizeof(prio_queue_elem_t));

  p_q->n_entries++;
  p_new->p_data = p_data;

  /* Insertion sort */
  while (p)
    {
      if (compare(p_data, p->p_data) < 0)
	{
	  /* Insert before p */
	  p_new->p_next = p;
	  if (!p_last)
	    p_q->p_first = p_new;
	  else
	    p_last->p_next = p_new;
	  return;
	}
      p_last = p;
      p = p->p_next;
    }

  /* Insert last (or first, ironically) */
  if (p_last)
    p_last->p_next = p_new;
  else
    p_q->p_first = p_new;
  p_new->p_next = NULL;
}

/* Pop the first entry of the queue */
void *prio_queue_get_first(prio_queue_t *p_q)
{
  prio_queue_elem_t *p_out = p_q->p_first;
  void *p_ret = NULL;

  if (p_out)
    {
      p_q->n_entries--;

      p_q->p_first = p_q->p_first->p_next;
      p_ret = p_out->p_data;
      free(p_out);
    }

  return p_ret;
}
