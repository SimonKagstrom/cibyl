/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      s9.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   t9-style library
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __S9_H__
#define __S9_H__

#include <stdint.h>

typedef struct
{
  int64_t  key;
  int      n_words;
  int      max_words;
  const char **words;        /* Unsorted, move-to-front semantics */
} s9_key_t;

/* This is actually internal */
typedef struct
{
  int        n_keys;
  int        max_keys;
  int        n_user_words;
  int        max_user_words;
  s9_key_t **keys;      /* Sorted by key */
} s9_wordlen_t;

/* ... as is this */
typedef struct
{
  int           expected_words;
  int           n_wordlens;
  s9_wordlen_t *words_by_length;
} s9_t;

/**
 * Initialize the S9 library.
 *
 * @param s9 the s9 object to initialize
 * @param expected_words the expected word-count (this is only a hint)
 *
 * @return 0 if everything was OK, < 0 otherwise
 */
int s9_init(s9_t *s9, int expected_words);

/**
 * Add a word to the S9 object
 *
 * @param s9 the s9 object to add the word to
 * @param word the word to add
 */
void s9_add_word(s9_t *s9, const char *word);

/**
 * Lookup a key in the database. This will initialize an iterator for
 * use with @a s9_lookup_next
 *
 * @param s9 the S9 object
 * @param sequence the null-terminated key sequence to lookup
 * @param n the number of entries for this key
 * @param it the iterator to fill in for this key
 * @return a pointer to the first string for this key, or NULL if there are
           no words for this key
 */
const char *s9_lookup(s9_t *s9, const char *sequence, int *n, void **it);

/**
 * Lookup the next word in the database.
 *
 * @param s9 the S9 object
 * @param it the iterator
 * @return a pointer to the next string for this key, or NULL if there are
           no more words for this key
 */
const char *s9_lookup_next(s9_t *s9, void **it);

#endif /* !__S9_H__ */
