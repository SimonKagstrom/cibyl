/*
  Static Malloc

  This is juts like malloc except it lacks sbrk() facilities.

  Instread, it is given a single, magically pre-allocated block of memory
  to work with.

  This code is taked from http://www.panix.com/~mbh/projects.html,
  and is apparently written by Mark B. Hansen.

  Since he didn't specify a license for this code, I cannot go ahead
  and GPL it.  Instead I offer it as free public domain.

  Calin A. Culianu <calin@ajvar.org>
*/


/*
** IDEAS:
**	- keep one link of the list for "sorted by address",
**	  another for for "sorted by size"
**	- mallopt options to turn NDEBUG, DISPLAY, etc. on and off
**	  and to change FIT function
**	  and to switch to functions with no error checking
**	  and to print statistics
**	- return freed memory over to OS if there's enough of it at the
**	  end of the segment
**	- X utility to show fragmentation
**	- specify which file to put output in
**	- output how many times each function has been called
*/
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define INT_MAX         ((int)(~0U>>1))
#define MAXINT INT_MAX
#define INT_MIN         (-INT_MAX - 1)
#define UINT_MAX        (~0U)
#define LONG_MAX        ((long)(~0UL>>1))
#define LONG_MIN        (-LONG_MAX - 1)
#define ULONG_MAX       (~0UL)

#define FALSE 0
#define TRUE 1


void smalloc_set_memory_pool(void *memory_start, void *memory_end);

/*
   Set this to use a specific printf function.. for diagnostic error messages.
   By default this is NULL so that there is no error logging.
*/
int do_printout = 0;

#define EMIT(x...) (do_printout ? fprintf(stderr, x) \
                    : (int)0)

#define FIT(x) bestfit(x) /**/

#define PATTERN(p) ((char)((unsigned int)(p) & 0xff))
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define ALIGN(p) (((p) + (sizeof(align_t) - 1)) & ~(sizeof(align_t) - 1))

#define ADDR(n) ((int)(n) + (int)sizeof(node))
#define AFTER(n) (ADDR(n) + (n)->x.rsize)
#define OVERHEAD (sizeof(node))

#define MINNODE (64 * sizeof(align_t))

#define ADJACENT(n1,n2) \
	((void *)(n1) + OVERHEAD + (n1)->x.rsize == (void *)(n2))



typedef long align_t;

union _node {
    struct {
        size_t rsize; /* real size */
        size_t usize; /* user size */
        union _node *next;
    } x;
    align_t align;
};
typedef union _node node;

enum _list {
    AVAILABLE,
    ALLOCATED
};
typedef enum _list list;


static node *available = NULL;
static node *allocated = NULL;
static unsigned long total = 0;
static unsigned long alloc_len = 0;
static unsigned long avail_len = 0;

static node *bestfit(size_t size);

/* put a breakpoint here for debugging */
static void debug() {
    total = total;
    if (0) {
      bestfit(0); /* avoid warnings */
    }
} /* debug */

static void delete(list l, node *n)
{
    node *c, *p, **head;

    head = (l == ALLOCATED) ? &allocated : &available;

    c = p = *head;
    while ((c != NULL) && (ADDR(c) > ADDR(n))) {
	p = c;
	c = c->x.next;
    }

    if (l == ALLOCATED) {
	alloc_len--;
    } else {
	avail_len--;
    }

    if (c == p) {
	*head = c->x.next;
    } else {
	p->x.next = c->x.next;
    }
} /* delete */


static void insert(list l, node *n)
{
    node *c, *p, **head;

    head = (l == ALLOCATED) ? &allocated : &available;

    c = p = *head;
    while ((c != NULL) && (ADDR(c) > ADDR(n))) {
      p = c;
      c = c->x.next;
    }

    if ((l == AVAILABLE) && (*head != NULL) && (c != NULL) && ADJACENT(c,n)) {
      c->x.rsize += OVERHEAD + n->x.rsize;
      if (ADJACENT(c,p)) {
	    delete(AVAILABLE, p);
	    c->x.rsize += OVERHEAD + p->x.rsize;
      }
    } else {
      if (l == ALLOCATED) {
	    alloc_len++;
      } else {
	    avail_len++;
      }
      n->x.next = c;
      if (c == p) {
	    *head = n;
      } else {
	    p->x.next = n;
	    if ((l == AVAILABLE) && (*head != NULL) && ADJACENT(n,p)) {
          delete(AVAILABLE, p);
          n->x.rsize += OVERHEAD + p->x.rsize;
	    }
      }
    }
} /* insert */


static node *find(node *head, void * ptr)
{
    node *c;

    c = head;
    while((c != NULL) && (ADDR(c) > (int)ptr)) {
	c = c->x.next;
    }

    if ((c == NULL) || (ADDR(c) != (int)ptr)) {
	return NULL;
    }

    return c;
} /* find */


static node *firstfit(size_t size)
{
    node *c;
    size_t aligned = ALIGN(size);

    c = available;
    while ((c != NULL) && ((int)c->x.rsize < (int)aligned)) {
      c = c->x.next;
    }

    if (c == NULL) {
      EMIT(__FILE__": OUT OF MEMORY!  Requested: %u  Total Allocated: %lu\n", 
           size, total);
      return NULL;
    } else {
      delete(AVAILABLE, c);
    }

    if ((int)c->x.rsize >= (int)(aligned + OVERHEAD + MINNODE)) {
      node *n;
      size_t leftover;
      
      leftover = c->x.rsize - aligned - OVERHEAD;
      c->x.rsize = aligned;
      
      n = (node *)AFTER(c);
      n->x.rsize = leftover;
      
      insert(AVAILABLE, n);

    }

    c->x.usize = size;

    insert(ALLOCATED, c);

    return c;
} /* firstfit */


static node *bestfit(size_t size)
{
    node *c, *b;
    size_t aligned = ALIGN(size);
    size_t over, best = MAXINT;

    b = NULL;
    c = available;
    while (c != NULL) {
	if (c->x.rsize >= size) {
	    over = c->x.rsize - size;
	    if ((over < best) || (c == available)) {
		best = over;
		b = c;
	    }
	}
	c = c->x.next;
    }

    if (b == NULL) {
      EMIT(__FILE__": OUT OF MEMORY!  Requested: %u  Total Allocated: %lu\n", 
           size, total);
      return NULL;
    } else {
      delete(AVAILABLE, b);
    }

    if (b->x.rsize >= aligned + OVERHEAD + MINNODE) {
	node *n;
	size_t leftover;
	
	leftover = b->x.rsize - aligned - OVERHEAD;
	b->x.rsize = aligned;

	n = (node *)AFTER(b);
	n->x.rsize = leftover;

	insert(AVAILABLE, n);

    }

    b->x.usize = size;

    insert(ALLOCATED, b);

    return b;
} /* bestfit */

/*---------------------------------------------------------------------------*/

static void * _malloc(size_t size)
{
    node *store;

    if (size == 0) {
      EMIT(__FILE__": attempt to allocate 0 bytes\n");
    }

    store = FIT(size);

    if (store == NULL) {
      EMIT(__FILE__": unable to allocate %u bytes\n", size);
      debug();
      return NULL;
    }

    total += size;
    return (void *)ADDR(store);
} /* _malloc */

static void _free(void * ptr)
{
    node *t;

    if (ptr == NULL) {
      EMIT(__FILE__": attempt to free NULL\n");
      return;
    }

    t = find(allocated, ptr);

    if (t == NULL) {
      EMIT(__FILE__": attempt to free unallocated memory (0x%x)\n",
           (unsigned int)ptr);
      debug();
      return;
    }

    delete(ALLOCATED, t);
    insert(AVAILABLE, t);

    total -= t->x.usize;
} /* _free */

/*---------------------------------------------------------------------------*/
/*----------------------------- USER INTERFACE: -----------------------------*/
/*---------------------------------------------------------------------------*/

void* malloc(size_t size)
{
    return _malloc(size);
} /* malloc */


void free(void *ptr)
{
    _free(ptr);
} /* free */


void smalloc_set_memory_pool(void *memory_start, void *memory_end)
{
  node *n = (node *)memory_start;
  size_t size = ADDR(memory_end) - ADDR(memory_start);

  n->x.rsize = size - OVERHEAD;
  n->x.next = NULL;

  allocated = NULL;
  available = n;
  total = 0;
  alloc_len = 0;
  avail_len = 1;
}
