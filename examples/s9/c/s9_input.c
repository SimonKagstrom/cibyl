/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      s9_input.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   S9 input test
 *
 * $Id:$
 *
 ********************************************************************/
#include <javax/microedition/lcdui.h>
#include <string.h>
#include <stdio.h>
#include <s9.h>
#include "s9_input.h"

typedef struct
{
  char input[81]; /* ~1 line of text */
  char output[81]; /* ~1 line of text */
  int in_ptr;
  int out_ptr;
  int word_start;

  void *it;
  int   n_matches;
  char *cur_word;

  s9_t *s9;
} s9_input_t;

static s9_input_t s9_in;

static void flush_output(void)
{
  if (s9_in.cur_word)
    {
      /* Flush word to output buf */
      s9_in.out_ptr += snprintf(s9_in.output + s9_in.out_ptr, 80 - s9_in.out_ptr,
                                "%s", s9_in.cur_word);
      s9_in.output[ s9_in.out_ptr++ ] = ' ';
      s9_in.output[ s9_in.out_ptr ] = '\0';
      printf("outbuf: %s\n", s9_in.output);

      s9_in.word_start = s9_in.in_ptr + 1;
      s9_in.cur_word = NULL;
    }
}

void s9_input_handle_keypress(int code)
{
  if (s9_in.in_ptr >= 80 || s9_in.out_ptr >= 80)
    return; /* Silently drop more */

  if (code == '#' && s9_in.cur_word) /* Space */
    {
      flush_output();
      code = ' ';
    }
  if (code == '*')
    {
      if (s9_in.n_matches > 1)
        s9_in.cur_word = s9_lookup_next(s9_in.s9, &s9_in.it);
      return;
    }
  if (code == '1')
    {
      s9_input_erase_one();
      return;
    }
  if (code == '0') /* Not used by s9 */
    return;

  s9_in.input[ s9_in.in_ptr++ ] = code;
  s9_in.input[ s9_in.in_ptr ] = '\0'; /* NULL-termination */

  s9_in.cur_word = s9_lookup(s9_in.s9, s9_in.input + s9_in.word_start, &s9_in.n_matches, &s9_in.it);
  printf("Adding %c to input, lookup: %s -> %s\n", code, s9_in.input + s9_in.word_start, s9_in.cur_word);
}

void s9_input_erase_one(void)
{
  if (s9_in.in_ptr > 0)
    s9_in.in_ptr--;
}

char *s9_input_get_translation(void)
{
  flush_output();
  return s9_in.output;
}

void s9_input_draw(NOPH_Graphics_t graphics, int x, int y)
{
  char out[81];

  if (s9_in.cur_word)
    snprintf(out, 81,
             "%s%s", s9_in.output, s9_in.cur_word);
  else
    {
      char buf[81];

      memset(buf, '?', s9_in.in_ptr - s9_in.out_ptr);
      buf[s9_in.in_ptr - s9_in.out_ptr] = '\0';

      snprintf(out, 81,
               "%s%s", s9_in.output, buf);
    }

  NOPH_Graphics_drawString(graphics,
                           out, x, y, NOPH_Graphics_TOP | NOPH_Graphics_LEFT);
}


void s9_input_reset(void)
{
  s9_in.in_ptr = 0;
  s9_in.word_start = 0;
  s9_in.out_ptr = 0;
  s9_in.cur_word = NULL;
  s9_in.n_matches = 0;
  s9_in.it = NULL;

  memset(s9_in.input, 0, 81);
  memset(s9_in.output, 0, 81);
}

void s9_input_init(s9_t *s9)
{
  s9_input_reset();
  s9_in.s9 = s9;
}
