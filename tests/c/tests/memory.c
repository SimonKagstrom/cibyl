/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      memory.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Memory tests
 *
 * $Id: memory.c 13859 2007-02-25 15:50:13Z ska $
 *
 ********************************************************************/
#include <stdint.h>
#include <test.h>

uint8_t vals[] =
{
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a
};

struct tjoho
{
  uint8_t a;
  int b;
} __attribute__((packed));

struct tjoho misalign = {0x1, 0x02030405};
struct tjoho misalign2 = {0x1, 1234};

uint8_t dst[10];

int memory_lwl(struct tjoho *src)
{
  return src->b;
}

void memory_swl(struct tjoho *dst, int val)
{
  dst->b = val;
}

/* The run-the-tests function */
void memory_run(void)
{
  struct tjoho tmp;
  char *a;
  int v;

  v = memory_lwl(&misalign);
  if ( v != 0x02030405 )
    FAIL("lwl: 0x%x != 0x%x\n", v, 0x02030405 );
  else
    PASS("lwl: 0x%x\n", v);

  v = memory_lwl(&misalign2);
  if ( v != 1234 )
    FAIL("lwl: 0x%x != 0x%x\n", v, 1234 );
  else
    PASS("lwl: 0x%x\n", v);

  memory_swl(&tmp, 0x02030405);
  a = (char*)&tmp.b;
  if (a[0] != 0x02 ||
      a[1] != 0x03 ||
      a[2] != 0x04 ||
      a[3] != 0x05)
    FAIL("swl: 0x%x != 0x%x\n", tmp.b, 0x02030405);
  else
    PASS("swl: 0x%x\n", tmp.b);
}
