/*********************************************************************
 *
 * Copyright (C) 2005,  Blekinge Institute of Technology
 *
 * Filename:      prio_queue.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Priority quue defs
 *
 * $Id: prio_queue.h 9722 2006-09-04 06:55:09Z ska $
 *
 ********************************************************************/
#ifndef __PRIO_QUEUE_H__
#define __PRIO_QUEUE_H__

typedef struct s_prio_queue_elem
{
  struct s_prio_queue_elem *p_next;
  void *p_data;
} prio_queue_elem_t;

typedef struct
{
  prio_queue_elem_t *p_first;
  int n_entries;
} prio_queue_t;

void prio_queue_init(prio_queue_t *p_q);

void prio_queue_insert(prio_queue_t *p_q, void *p_data, int (*compare)(const void *pa, const void *pb) );

void *prio_queue_get_first(prio_queue_t *p_q);

#endif /* !__PRIO_QUEUE_H__ */
