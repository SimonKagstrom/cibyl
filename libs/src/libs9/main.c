/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      main.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   t9 test program
 *
 * $Id:$
 *
 ********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <s9.h>

static void usage(void)
{
  printf("Usage: s9 wordlist seq [seq2...]\n");
  exit(1);
}

int main(int argc, char *argv[])
{
  char buf[255];
  char *line;
  FILE *fp;
  s9_t s9;
  int i;

  if (argc < 3)
    usage();

  s9_init(&s9, 128);

  if ( !(fp = fopen(argv[1], "r")) )
    usage();

  while ( !feof(fp) )
    {
      line = fgets(buf, 255, fp);
      if (!line)
        break;
      line[strlen(line)-1] = '\0';

      s9_add_word(&s9, strdup(line));
    }

  for ( i = 2; i < argc; i++ )
    {
      const char *cur;
      void *it;
      int n;

      printf("Lookup %s\n", argv[i]);
      cur = s9_lookup(&s9, argv[i], &n, &it);
      if (!cur)
        continue;
      do
        {
          printf("  %s:%p\n", cur, it);
          cur = s9_lookup_next(&s9, &it);
        } while(cur);
    }

  fclose(fp);

  return 0;
}
