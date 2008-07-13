#include <stdlib.h>

int add_to_arg(int num)
{
  return num + 1;
}

int *get_int_pointer(void)
{
  return (int*)malloc(sizeof(int*));
}

void update_pointer(int *p)
{
  *p = 99;
}
