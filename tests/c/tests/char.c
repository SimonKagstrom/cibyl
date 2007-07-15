/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      int.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Integer tests
 *
 * $Id: char.c 13078 2007-01-18 06:06:09Z ska $
 *
 ********************************************************************/
#include <test.h>
#include <stdlib.h>

/* Functions to test */
binary_arithmetic_test_char(char_add, +);
binary_arithmetic_test_char(char_sub, -);
binary_arithmetic_test_char(char_mul, *);
binary_arithmetic_test_char(char_div, /);
binary_arithmetic_test_char(char_mod, %);
binary_arithmetic_test_char(char_xor, ^);
binary_arithmetic_test_char(char_or,  |);
binary_arithmetic_test_char(char_and, &);
binary_arithmetic_test_char(char_shl, <<);
binary_arithmetic_test_char(char_shr, >>);
binary_arithmetic_test_char(char_ge, >=);
binary_arithmetic_test_char(char_lt, <);
binary_arithmetic_test_char(char_le, <=);
binary_arithmetic_test_char(char_gt, >);
binary_arithmetic_test_char(char_eq, ==);
binary_arithmetic_test_char(char_ne, !=);

binary_arithmetic_test_unsigned_char(unsigned_char_add, +);
binary_arithmetic_test_unsigned_char(unsigned_char_sub, -);
binary_arithmetic_test_unsigned_char(unsigned_char_mul, *);
binary_arithmetic_test_unsigned_char(unsigned_char_div, /);
binary_arithmetic_test_unsigned_char(unsigned_char_mod, %);
binary_arithmetic_test_unsigned_char(unsigned_char_xor, ^);
binary_arithmetic_test_unsigned_char(unsigned_char_or,  |);
binary_arithmetic_test_unsigned_char(unsigned_char_and, &);
binary_arithmetic_test_unsigned_char(unsigned_char_shl, <<);
binary_arithmetic_test_unsigned_char(unsigned_char_shr, >>);
binary_arithmetic_test_unsigned_char(unsigned_char_ge, >=);
binary_arithmetic_test_unsigned_char(unsigned_char_lt, <);
binary_arithmetic_test_unsigned_char(unsigned_char_le, <=);
binary_arithmetic_test_unsigned_char(unsigned_char_gt, >);
binary_arithmetic_test_unsigned_char(unsigned_char_eq, ==);
binary_arithmetic_test_unsigned_char(unsigned_char_ne, !=);

/* The actual tests (add more here, especially regressions) */
static char_test_t char_tests[] =
{
  BIN_OP(char_add, +, 5, 9),
  BIN_OP(char_add, +, -1, -1),
  BIN_OP(char_add, +, 0, 0),
  BIN_OP(char_add, +, -1, 1),
  BIN_OP(char_add, +, 1, 128),
  BIN_OP(char_add, +, 1, 127),
  BIN_OP(char_add, +, 2, 127),
  BIN_OP(char_add, +, -1, 128),
  BIN_OP(char_add, +, 128, -128),

  BIN_OP(char_sub, -, 5, 9),
  BIN_OP(char_sub, -, -1, -1),
  BIN_OP(char_sub, -, 0, 0),
  BIN_OP(char_sub, -, -1, 127),

  BIN_OP(char_mul, *, 5, 9),
  BIN_OP(char_mul, *, 5, 0),
  BIN_OP(char_mul, *, 12, -0),
  BIN_OP(char_mul, *, 166, -1),
  BIN_OP(char_mul, *, 127, 2),
  BIN_OP(char_mul, *, -1, -1),

  BIN_OP(char_div, /, -1, -1),
  BIN_OP(char_div, /, -1, 5),
  BIN_OP(char_div, /,  1, -1),
  BIN_OP(char_div, /,  127, 2),

  BIN_OP(char_mod, %,  -2, -3),
  BIN_OP(char_mod, %,  0, 1),
  BIN_OP(char_mod, %,  1, 1),
  BIN_OP(char_mod, %,  4, -1),

  BIN_OP(char_and, &,  0xff, 1),
  BIN_OP(char_and, &,  1, 0xff),
  BIN_OP(char_and, &,  0, 0xff),
  BIN_OP(char_and, &,  2, 3),

  BIN_OP(char_or, |,  2, 3),
  BIN_OP(char_or, |,  2, -1),
  BIN_OP(char_or, |,  0, -1),
  BIN_OP(char_or, |,  1, 2),

  BIN_OP(char_xor, ^,  3, 2),
  BIN_OP(char_xor, ^,  3, -1),
  BIN_OP(char_xor, ^,  -1, -1),
  BIN_OP(char_xor, ^,  0, 0),

  BIN_OP(char_shr, >>,  0x10, 0x10),
  BIN_OP(char_shr, >>,  0x10, 1),
  BIN_OP(char_shr, >>,  1, 1),
  BIN_OP(char_shr, >>,  1, 0),
  BIN_OP(char_shr, >>,  0, 0),

  BIN_OP(char_shl, <<,  0x10, 4),
  BIN_OP(char_shl, <<,  0x10, 1),
  BIN_OP(char_shl, <<,  -1, 1),
  BIN_OP(char_shl, <<,  0, 31),
  BIN_OP(char_shl, <<,  0, 0),


  BIN_OP(char_ge, >=, -1, 1),
  BIN_OP(char_ge, >=, 1,  1),
  BIN_OP(char_ge, >=, 1, -1),

  BIN_OP(char_lt, <, -1,  1),
  BIN_OP(char_lt, <,  1,  1),
  BIN_OP(char_lt, <,  1, -1),

  BIN_OP(char_le, <=,-1,  1),
  BIN_OP(char_le, <=, 1,  1),
  BIN_OP(char_le, <=, 1, -1),

  BIN_OP(char_gt, >, -1,  1),
  BIN_OP(char_gt, >,  1,  1),
  BIN_OP(char_gt, >,  1, -1),

  BIN_OP(char_eq, ==,-1,  1),
  BIN_OP(char_eq, ==, 1,  1),
  BIN_OP(char_eq, ==, 1, -1),
  BIN_OP(char_eq, ==, 0, 0),
  BIN_OP(char_eq, ==,-0, -0),

  BIN_OP(char_ne, !=, 1, -1),
  BIN_OP(char_ne, !=, 0, 0),
};


/* The actual tests (add more here, especially regressions) */
static unsigned_char_test_t unsigned_char_tests[] =
{
  BIN_OP_UNSIGNED(unsigned_char_add, +, 5, 9),
  BIN_OP_UNSIGNED(unsigned_char_add, +, 254, 31),
  BIN_OP_UNSIGNED(unsigned_char_add, +, 0, 0),
  BIN_OP_UNSIGNED(unsigned_char_add, +, 1, 1),
  BIN_OP_UNSIGNED(unsigned_char_add, +, 1, 255),
  BIN_OP_UNSIGNED(unsigned_char_add, +, 1, 254),
  BIN_OP_UNSIGNED(unsigned_char_add, +, 2, 127),
  BIN_OP_UNSIGNED(unsigned_char_add, +, -1, 255),
  BIN_OP_UNSIGNED(unsigned_char_add, +, 255, -255),

  BIN_OP_UNSIGNED(unsigned_char_sub, -, 5, 9),
  BIN_OP_UNSIGNED(unsigned_char_sub, -, -1, -1),
  BIN_OP_UNSIGNED(unsigned_char_sub, -, 0, 0),
  BIN_OP_UNSIGNED(unsigned_char_sub, -, -1, 127),

  BIN_OP_UNSIGNED(unsigned_char_mul, *, 5, 9),
  BIN_OP_UNSIGNED(unsigned_char_mul, *, 5, 0),
  BIN_OP_UNSIGNED(unsigned_char_mul, *, 12, -0),
  BIN_OP_UNSIGNED(unsigned_char_mul, *, 127, 1),
  BIN_OP_UNSIGNED(unsigned_char_mul, *, 127, 2),
  BIN_OP_UNSIGNED(unsigned_char_mul, *, 61, 13),

  BIN_OP_UNSIGNED(unsigned_char_div, /, -1, -1),
  BIN_OP_UNSIGNED(unsigned_char_div, /, -1, 5),
  BIN_OP_UNSIGNED(unsigned_char_div, /,  1, -1),
  BIN_OP_UNSIGNED(unsigned_char_div, /,  127, 2),

  BIN_OP_UNSIGNED(unsigned_char_mod, %,  -2, -3),
  BIN_OP_UNSIGNED(unsigned_char_mod, %,  0, 1),
  BIN_OP_UNSIGNED(unsigned_char_mod, %,  1, 1),
  BIN_OP_UNSIGNED(unsigned_char_mod, %,  4, -1),

  BIN_OP_UNSIGNED(unsigned_char_and, &,  0xff, 1),
  BIN_OP_UNSIGNED(unsigned_char_and, &,  1, 0xff),
  BIN_OP_UNSIGNED(unsigned_char_and, &,  0, 0xff),
  BIN_OP_UNSIGNED(unsigned_char_and, &,  2, 3),

  BIN_OP_UNSIGNED(unsigned_char_or, |,  2, 3),
  BIN_OP_UNSIGNED(unsigned_char_or, |,  2, 99),
  BIN_OP_UNSIGNED(unsigned_char_or, |,  0, 15),
  BIN_OP_UNSIGNED(unsigned_char_or, |,  1, 2),

  BIN_OP_UNSIGNED(unsigned_char_xor, ^,  3, 2),
  BIN_OP_UNSIGNED(unsigned_char_xor, ^,  99, 11),
  BIN_OP_UNSIGNED(unsigned_char_xor, ^,  -1, -1),
  BIN_OP_UNSIGNED(unsigned_char_xor, ^,  0, 0),

  BIN_OP_UNSIGNED(unsigned_char_shr, >>,  0x10, 0x10),
  BIN_OP_UNSIGNED(unsigned_char_shr, >>,  0x10, 1),
  BIN_OP_UNSIGNED(unsigned_char_shr, >>,  1, 1),
  BIN_OP_UNSIGNED(unsigned_char_shr, >>,  1, 0),
  BIN_OP_UNSIGNED(unsigned_char_shr, >>,  0, 0),

  BIN_OP_UNSIGNED(unsigned_char_shl, <<,  0x10, 4),
  BIN_OP_UNSIGNED(unsigned_char_shl, <<,  0x10, 1),
  BIN_OP_UNSIGNED(unsigned_char_shl, <<,  0xff, 1),
  BIN_OP_UNSIGNED(unsigned_char_shl, <<,  0, 31),
  BIN_OP_UNSIGNED(unsigned_char_shl, <<,  0, 0),


  BIN_OP_UNSIGNED(unsigned_char_ge, >=, -1, 1),
  BIN_OP_UNSIGNED(unsigned_char_ge, >=, 1,  1),
  BIN_OP_UNSIGNED(unsigned_char_ge, >=, 1, -1),

  BIN_OP_UNSIGNED(unsigned_char_lt, <, -1,  1),
  BIN_OP_UNSIGNED(unsigned_char_lt, <,  1,  1),
  BIN_OP_UNSIGNED(unsigned_char_lt, <,  1, -1),

  BIN_OP_UNSIGNED(unsigned_char_le, <=,-1,  1),
  BIN_OP_UNSIGNED(unsigned_char_le, <=, 1,  1),
  BIN_OP_UNSIGNED(unsigned_char_le, <=, 1, -1),

  BIN_OP_UNSIGNED(unsigned_char_gt, >, -1,  1),
  BIN_OP_UNSIGNED(unsigned_char_gt, >,  1,  1),
  BIN_OP_UNSIGNED(unsigned_char_gt, >,  1, -1),

  BIN_OP_UNSIGNED(unsigned_char_eq, ==,-1,  1),
  BIN_OP_UNSIGNED(unsigned_char_eq, ==, 1,  1),
  BIN_OP_UNSIGNED(unsigned_char_eq, ==, 1, -1),
  BIN_OP_UNSIGNED(unsigned_char_eq, ==, 0, 0),
  BIN_OP_UNSIGNED(unsigned_char_eq, ==,-0, -0),

  BIN_OP_UNSIGNED(unsigned_char_ne, !=, 1, -1),
  BIN_OP_UNSIGNED(unsigned_char_ne, !=, 0, 0),
};


/* The run-the-tests function */
void char_run(void)
{
  run_test_bin_vector(char, char_test_t, char_tests, "%d");
  run_test_bin_vector(unsigned char, unsigned_char_test_t, unsigned_char_tests, "%u");
}
