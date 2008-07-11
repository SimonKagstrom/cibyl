#include <stdio.h>

int abc=4;
int def;

int tst(int a)
{
  if (a < 4)
    return a-1;
  return tst(a-1) * a;
}

int main(int argc, char **argv)
{
  printf("Hello, cruel world %d!\n", tst(4));
  return 0;
}
