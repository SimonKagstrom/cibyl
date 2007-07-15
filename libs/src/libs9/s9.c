/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      s9.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   T9-style library
 *
 * $Id:$
 *
 ********************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <s9.h>

#define abort_on(x) do { \
   if ( (x) ) {          \
       printf("Aborting on " #x); \
       exit(1);          \
   }                     \
}while(0)

/* Get the expected number of words of a certain length. Apply some
 * logic here
 */
static int get_expected_words(int total, int len)
{
  return 8;
}

static int get_key_by_char(char c)
{
  c = tolower(c);

  if (c >= 'a' && c <= 'c')
    return 2;
  if (c >= 'd' && c <= 'f')
    return 3;
  if (c >= 'g' && c <= 'i')
    return 4;
  if (c >= 'j' && c <= 'l')
    return 5;
  if (c >= 'm' && c <= 'o')
    return 6;
  if (c >= 'p' && c <= 's')
    return 7;
  if (c >= 't' && c <= 'v')
    return 8;
  if (c >= 'w' && c <= 'z')
    return 9;

  return -1;
}

static int64_t get_key(const char *word)
{
  int64_t out = 0;
  int len = strlen(word);
  int i;

  if (len > (sizeof(int64_t) * 8) / 3)
    return -1;

  for (i = 0; i < len; i++)
    {
      int c = get_key_by_char(word[i]);

      if (c < 0)
        return -1;
      c -= 2; /* between 0..7, fits in 3 bits */
      out |= (int64_t)c << (i*3);
    }

  return out;
}

/**
 * Return the key value for a sequence of characters
 *
 * @param sequence a NULL-terminated string of the characters 2..9
 * @return the key value for the sequence or -1 if it contains invalid
 *         characters
 */
static int64_t get_key_by_sequence(const char *sequence)
{
  int64_t out = 0;
  int len = strlen(sequence);
  const char *p;
  int i;

  if (len > (sizeof(int64_t) * 8) / 3)
    return -1;

  for (p = sequence, i = 0;
       *p;
       p++, i++)
    {
      int64_t cur = *p;

      if (cur < '2' || cur > '9')
        return -1;

      out |= (cur - '2') << (i*3);
    }

  return out;
}

static s9_key_t *key_new(int64_t key, int max_words)
{
  s9_key_t *out;

  out = (s9_key_t*)malloc(sizeof(s9_key_t));
  abort_on(!out);
  out->key = key;
  out->n_words = 0;
  out->max_words = max_words;

  out->words = (const char**)malloc(sizeof(const char*) * max_words);
  abort_on(!out->words);
  memset(out->words, 0, sizeof(const char*) * max_words);

  return out;
}

static void key_add_word(s9_key_t *key, const char *word)
{
  int i;

  for (i = 0; i < key->n_words; i++)
    {
      /* Already there */
      if ( strcmp(key->words[i], word) == 0 )
        return;
    }
  if (key->n_words + 1 > key->max_words)
    {
      key->max_words *= 2;

      /* Add space for null-termination */
      key->words = (const char**)realloc(key->words, sizeof(const char*) * (key->max_words + 1));
      abort_on(!key->words);
      memset(&key->words[key->n_words], 0, sizeof(const char*) * (key->max_words / 2 + 1));
    }
  key->words[key->n_words] = word;
  key->n_words++;
}

static void wordlen_init(s9_wordlen_t *wl, int max_keys)
{
  wl->n_keys = 0;
  wl->max_keys = max_keys;
  wl->n_user_words = 0;
  wl->max_user_words = 0; /* Fix this later */

  wl->keys = (s9_key_t**)malloc(sizeof(s9_key_t*) * wl->max_keys);
  abort_on(!wl->keys);
  memset(wl->keys, 0, sizeof(s9_key_t*) * wl->max_keys);
}

static int wordlen_lookup_key(s9_wordlen_t *wl, int64_t key)
{
  int i;

  /* Search through the list for keys */
  for (i = 0; i < wl->n_keys; i++)
    {
      if (wl->keys[i]->key == key)
        return i;
    }

  return -1;
}

static void wordlen_add_word(s9_wordlen_t *wl, const char *word)
{
  int64_t key = get_key(word);
  int idx;

  /* Silently fail on non-representable words */
  if (key < 0)
    return;

  idx = wordlen_lookup_key(wl, key);
  if (idx < 0)
    {
      /* Not found, add space for more keys */
      if (wl->n_keys+1 >= wl->max_keys)
        {
          wl->max_keys *= 2;
          wl->keys = (s9_key_t**)realloc(wl->keys, sizeof(s9_key_t*) * wl->max_keys);
          memset(&wl->keys[wl->max_keys / 2], 0, sizeof(s9_key_t*) * (wl->max_keys / 2));
          abort_on(!wl->keys);
        }
      idx = wl->n_keys;
      wl->keys[idx] = key_new(key, 8);
      wl->n_keys++;
    }

  /* Add the word to this key */
  key_add_word(wl->keys[idx], word);
}

int s9_init(s9_t *s9, int expected_words)
{
  s9->n_wordlens = 0;
  s9->expected_words = expected_words;
  s9->words_by_length = NULL;

  return 0;
}

void s9_add_word(s9_t *s9, const char *word)
{
  int len = strlen(word);

  /* Do we need more space? */
  if (len >= s9->n_wordlens)
    {
      int i;

      s9->words_by_length = (s9_wordlen_t*)realloc(s9->words_by_length, sizeof(s9_wordlen_t) * (len + 1));
      abort_on(!s9->words_by_length);
      for (i = s9->n_wordlens; i < len + 1; i++)
        wordlen_init(&s9->words_by_length[i], get_expected_words(len, s9->expected_words) );
      s9->n_wordlens = len + 1;
    }

  wordlen_add_word(&s9->words_by_length[len], word);
}

const char *s9_lookup(s9_t *s9, const char *sequence, int *n, void **it)
{
  int64_t key = get_key_by_sequence(sequence);
  int len = strlen(sequence);
  s9_wordlen_t *wl;
  int idx;

  if (key < 0)
    return NULL;
  wl = &s9->words_by_length[len];

  idx = wordlen_lookup_key(wl, key);
  if (idx < 0)
    return NULL;

  *it = (void*)wl->keys[idx]->words;
  *n = wl->keys[idx]->n_words;

  return wl->keys[idx]->words[0];
}

const char *s9_lookup_next(s9_t *s9, void **it)
{
  const char **p = *it;

  p++;
  *it = p;

  return *p;
}
