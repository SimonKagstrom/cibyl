#include <test.h>

struct big_struct
{
  unsigned int v[8];
};

static void __attribute__((noinline))with_big_struct(struct big_struct s)
{
	if (s.v[0] != 0x11111111 || s.v[1] != 0x22222222 || s.v[2] != 0x33333333 || s.v[3] != 0x44444444 ||
			s.v[4] != 0x55555555 || s.v[5] != 0x66666666 || s.v[6] != 0x77777777 || s.v[7] != 0x88888888)
		FAIL("pass-struct-by-val: %08x:%08x:%08x:%08x:%08x:%08x:%08x:%08x",
				s.v[0], s.v[1], s.v[2], s.v[3], s.v[4], s.v[5], s.v[6], s.v[7]);
	else
		PASS("pass-struct-by-val: %08x:%08x:%08x:%08x:%08x:%08x:%08x:%08x",
				s.v[0], s.v[1], s.v[2], s.v[3], s.v[4], s.v[5], s.v[6], s.v[7]);
}

static void pass_big_struct(void)
{
  struct big_struct s;

  s.v[0] = 0x11111111;
  s.v[1] = 0x22222222;
  s.v[2] = 0x33333333;
  s.v[3] = 0x44444444;
  s.v[4] = 0x55555555;
  s.v[5] = 0x66666666;
  s.v[6] = 0x77777777;
  s.v[7] = 0x88888888;

  with_big_struct(s);
}

void structs_by_value_run(void)
{
	pass_big_struct();
}
