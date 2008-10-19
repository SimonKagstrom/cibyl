#include <stdio.h>
#include <cibyl.h>

int abc=4;
int def;

#include <setjmp.h>
jmp_buf maboo1;
int tjoho(void)
{
  printf("Tjoho\n");
  longjmp(maboo1, 100);
  return 0;
}

int tst(int a)
{
  tjoho();
  if (a < 4)
    return a-1;
  return tst(a-1) * a;
}

int main(int argc, char **argv)
{
  int v2;
  int v = setjmp(maboo1);
  if ( v == 0 )
    {
      printf("Maboo 0\n");
    }
  else
    {
      printf("Maboo 1\n");
      return 1;
    }

  printf("Hello, cruel world %d!\n", tst(4));
  return 0;
}
