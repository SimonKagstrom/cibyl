/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      int.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Integer tests
 *
 * $Id: int.c 13073 2007-01-17 19:02:21Z ska $
 *
 ********************************************************************/
#include <test.h>
#include <stdlib.h>

/* Functions to test */
binary_arithmetic_test_int(int_add, +);
binary_arithmetic_test_int(int_sub, -);
binary_arithmetic_test_int(int_mul, *);
binary_arithmetic_test_int(int_div, /);
binary_arithmetic_test_int(int_mod, %);
binary_arithmetic_test_int(int_xor, ^);
binary_arithmetic_test_int(int_or,  |);
binary_arithmetic_test_int(int_and, &);
binary_arithmetic_test_int(int_shl, <<);
binary_arithmetic_test_int(int_shr, >>);
binary_arithmetic_test_int(int_ge, >=);
binary_arithmetic_test_int(int_lt, <);
binary_arithmetic_test_int(int_le, <=);
binary_arithmetic_test_int(int_gt, >);
binary_arithmetic_test_int(int_eq, ==);
binary_arithmetic_test_int(int_ne, !=);

/* The actual tests (add more here, especially regressions) */
static int_test_t int_tests[] =
{
  BIN_OP(int_add, +, 5, 9),
  BIN_OP(int_add, +, -1, -1),
  BIN_OP(int_add, +, 0, 0),
  BIN_OP(int_add, +, -1, 1),
  BIN_OP(int_add, +, 1, 0xffffffff),
  BIN_OP(int_add, +, 1231565, -12312565),

  BIN_OP(int_sub, -, 5, 9),
  BIN_OP(int_sub, -, -1, -1),
  BIN_OP(int_sub, -, 0, 0),
  BIN_OP(int_sub, -, 3151555, 124315),

  BIN_OP(int_mul, *, 5, 9),
  BIN_OP(int_mul, *, 5, 0),
  BIN_OP(int_mul, *, 1255, -0),
  BIN_OP(int_mul, *, 6979, -1),
  BIN_OP(int_mul, *, 0x7fffffff, 2),
  BIN_OP(int_mul, *, -1, -1),

  BIN_OP(int_div, /, -1, -1),
  BIN_OP(int_div, /, -1, 5),
  BIN_OP(int_div, /,  1, 414141),
  BIN_OP(int_div, /,  1, -1),
  BIN_OP(int_div, /,  26, 25),
  BIN_OP(int_div, /,  2, 25),
  BIN_OP(int_div, /,  40, 30),
  BIN_OP(int_div, /,  155525, 536),

  BIN_OP(int_mod, %,  155525, 536),
  BIN_OP(int_mod, %,  15, 15),
  BIN_OP(int_mod, %,  -15, -10),
  BIN_OP(int_mod, %,  -1, 25),
  BIN_OP(int_mod, %,  26, 25),
  BIN_OP(int_mod, %,  2, 25),
  BIN_OP(int_mod, %,  199, 25),
  BIN_OP(int_mod, %,  0, 1),
  BIN_OP(int_mod, %,  1, 1),
  BIN_OP(int_mod, %,  4, -1),

  BIN_OP(int_and, &,  0xffffffff, 1),
  BIN_OP(int_and, &,  1, 0xffffffff),
  BIN_OP(int_and, &,  0, 0xffffffff),
  BIN_OP(int_and, &,  2, 3),

  BIN_OP(int_or, |,  2, 3),
  BIN_OP(int_or, |,  2, -1),
  BIN_OP(int_or, |,  0, -1),
  BIN_OP(int_or, |,  1, 2),

  BIN_OP(int_xor, ^,  3, 2),
  BIN_OP(int_xor, ^,  3, -1),
  BIN_OP(int_xor, ^,  -1, -1),
  BIN_OP(int_xor, ^,  0, 0),

  BIN_OP(int_shr, >>,  0x10, 0x10),
  BIN_OP(int_shr, >>,  0x10, 1),
  BIN_OP(int_shr, >>,  1, 1),
  BIN_OP(int_shr, >>,  1, 0),
  BIN_OP(int_shr, >>,  0, 0),

  BIN_OP(int_shl, <<,  0x10, 0x10),
  BIN_OP(int_shl, <<,  0x10, 1),
  BIN_OP(int_shl, <<,  -1, 1),
  BIN_OP(int_shl, <<,  0, 31),
  BIN_OP(int_shl, <<,  0, 0),


  BIN_OP(int_ge, >=, -1, 1),
  BIN_OP(int_ge, >=, 1,  1),
  BIN_OP(int_ge, >=, 1, -1),

  BIN_OP(int_lt, <, -1,  1),
  BIN_OP(int_lt, <,  1,  1),
  BIN_OP(int_lt, <,  1, -1),

  BIN_OP(int_le, <=,-1,  1),
  BIN_OP(int_le, <=, 1,  1),
  BIN_OP(int_le, <=, 1, -1),

  BIN_OP(int_gt, >, -1,  1),
  BIN_OP(int_gt, >,  1,  1),
  BIN_OP(int_gt, >,  1, -1),

  BIN_OP(int_eq, ==,-1,  1),
  BIN_OP(int_eq, ==, 1,  1),
  BIN_OP(int_eq, ==, 1, -1),
  BIN_OP(int_eq, ==, 0, 0),
  BIN_OP(int_eq, ==,-0, -0),

  BIN_OP(int_ne, !=, 1, -1),
  BIN_OP(int_ne, !=, 0, 0),
};


/* The run-the-tests function */
void int_run(void)
{
  run_test_bin_vector(int, int_test_t, int_tests, "%d");
}
