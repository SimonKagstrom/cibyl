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
binary_arithmetic_test_longlong(longlong_add, +);
binary_arithmetic_test_longlong(longlong_sub, -);
binary_arithmetic_test_longlong(longlong_mul, *);
binary_arithmetic_test_longlong(longlong_div, /);
binary_arithmetic_test_longlong(longlong_mod, %);
binary_arithmetic_test_longlong(longlong_xor, ^);
binary_arithmetic_test_longlong(longlong_or,  |);
binary_arithmetic_test_longlong(longlong_and, &);
binary_arithmetic_test_longlong(longlong_shl, <<);
binary_arithmetic_test_longlong(longlong_shr, >>);
binary_arithmetic_test_longlong(longlong_ge, >=);
binary_arithmetic_test_longlong(longlong_lt, <);
binary_arithmetic_test_longlong(longlong_le, <=);
binary_arithmetic_test_longlong(longlong_gt, >);
binary_arithmetic_test_longlong(longlong_eq, ==);
binary_arithmetic_test_longlong(longlong_ne, !=);

/* The actual tests (add more here, especially regressions) */
static longlong_test_t longlong_tests[] =
{
  BIN_OP(longlong_add, +, 5, 9),
  BIN_OP(longlong_add, +, -1, -1),
  BIN_OP(longlong_add, +, 0, 0),
  BIN_OP(longlong_add, +, -1, 1),
  BIN_OP(longlong_add, +, 1, 0xffffffff),
  BIN_OP(longlong_add, +, 1231565, -12312565),

  BIN_OP(longlong_sub, -, 5, 9),
  BIN_OP(longlong_sub, -, -1, -1),
  BIN_OP(longlong_sub, -, 0, 0),
  BIN_OP(longlong_sub, -, 3151555, 124315),

  BIN_OP(longlong_mul, *, 5, 9),
  BIN_OP(longlong_mul, *, 5, 0),
  BIN_OP(longlong_mul, *, 1255, -0),
  BIN_OP(longlong_mul, *, 6979, -1),
  BIN_OP(longlong_mul, *, 0x7fffffff, 2),
  BIN_OP(longlong_mul, *, -1, -1),

  BIN_OP(longlong_div, /, -1, -1),
  BIN_OP(longlong_div, /, -1, 5),
  BIN_OP(longlong_div, /,  1, 414141),
  BIN_OP(longlong_div, /,  1, -1),
  BIN_OP(longlong_div, /,  26, 25),
  BIN_OP(longlong_div, /,  2, 25),
  BIN_OP(longlong_div, /,  40, 30),
  BIN_OP(longlong_div, /,  155525, 536),

  BIN_OP(longlong_mod, %,  155525, 536),
  BIN_OP(longlong_mod, %,  15, 15),
  BIN_OP(longlong_mod, %,  -15, -10),
  BIN_OP(longlong_mod, %,  -1, 25),
  BIN_OP(longlong_mod, %,  26, 25),
  BIN_OP(longlong_mod, %,  2, 25),
  BIN_OP(longlong_mod, %,  199, 25),
  BIN_OP(longlong_mod, %,  0, 1),
  BIN_OP(longlong_mod, %,  1, 1),
  BIN_OP(longlong_mod, %,  4, -1),

  BIN_OP(longlong_and, &,  0xffffffff, 1),
  BIN_OP(longlong_and, &,  1, 0xffffffff),
  BIN_OP(longlong_and, &,  1234, 0xffffffff),
  BIN_OP(longlong_and, &,  1234, 0xffff),
  BIN_OP(longlong_and, &,  0, 0xffffffff),
  BIN_OP(longlong_and, &,  2, 3),

  BIN_OP(longlong_or, |,  2, 3),
  BIN_OP(longlong_or, |,  2, -1),
  BIN_OP(longlong_or, |,  0, -1),
  BIN_OP(longlong_or, |,  1, 2),

  BIN_OP(longlong_xor, ^,  3, 2),
  BIN_OP(longlong_xor, ^,  3, -1),
  BIN_OP(longlong_xor, ^,  -1, -1),
  BIN_OP(longlong_xor, ^,  0, 0),

  BIN_OP(longlong_shr, >>,  0x10, 0x10),
  BIN_OP(longlong_shr, >>,  0x10, 1),
  BIN_OP(longlong_shr, >>,  1, 1),
  BIN_OP(longlong_shr, >>,  1, 0),
  BIN_OP(longlong_shr, >>,  0, 0),

  BIN_OP(longlong_shl, <<,  0x10, 0x10),
  BIN_OP(longlong_shl, <<,  0x10, 1),
  BIN_OP(longlong_shl, <<,  -1, 1),
  BIN_OP(longlong_shl, <<,  0, 31),
  BIN_OP(longlong_shl, <<,  0, 0),


  BIN_OP(longlong_ge, >=, -1, 1),
  BIN_OP(longlong_ge, >=, 1,  1),
  BIN_OP(longlong_ge, >=, 1, -1),

  BIN_OP(longlong_lt, <, -1,  1),
  BIN_OP(longlong_lt, <,  1,  1),
  BIN_OP(longlong_lt, <,  1, -1),

  BIN_OP(longlong_le, <=,-1,  1),
  BIN_OP(longlong_le, <=, 1,  1),
  BIN_OP(longlong_le, <=, 1, -1),

  BIN_OP(longlong_gt, >, -1,  1),
  BIN_OP(longlong_gt, >,  1,  1),
  BIN_OP(longlong_gt, >,  1, -1),

  BIN_OP(longlong_eq, ==,-1,  1),
  BIN_OP(longlong_eq, ==, 1,  1),
  BIN_OP(longlong_eq, ==, 1, -1),
  BIN_OP(longlong_eq, ==, 0, 0),
  BIN_OP(longlong_eq, ==,-0, -0),

  BIN_OP(longlong_ne, !=, 1, -1),
  BIN_OP(longlong_ne, !=, 0, 0),
};

/* The run-the-tests function */
void longlong_run(void)
{
  run_test_bin_vector(long long, longlong_test_t, longlong_tests, "%Ld");
}
