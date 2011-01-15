/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      utils.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Misc utils to have it compile on all platforms
 *
 * $Id: utils.c 13453 2007-02-05 16:28:37Z ska $
 *
 ********************************************************************/
#include "utils.h"

int charbuf_to_int(char *buf, int digits)
{
  int i;
  int out = 0;

  for (i = 0; i < digits; i++)
    {
      int cur = (buf[i] - '0') << ((digits - i - 1) * 4);

      out |= cur;
    }

  return out;
}


void int_to_charbuf(unsigned int val, char *buf, int digits)
{
  int i;

  if (val > (0xf << (digits*4))-1)
    {
      /* Do something more intelligent here, maybe even exit */
      printf("Value %d too large to represent in %d digits\n", val, digits);
      return;
    }

  for (i = 0; i < digits; i++)
    {
      int cur = val & (0xf << ( (digits - i - 1) * 4 ));
      cur >>= ((digits - i - 1)*4) ;

      buf[i] = '0' + cur;
    }
}
