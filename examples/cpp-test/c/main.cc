#include <stdio.h>

class Tjoho
{
public:
  Tjoho(int a)
  {
    printf("Constructor: %d\n", a);
    this->value = a;
  }

  int getValue();

private:
  int value;
};

int Tjoho::getValue()
{
  printf("getValue: %d\n", this->value);
  return this->value;
}

int main(int argc, char **argv)
{
  Tjoho t(9);
  Tjoho *p = new Tjoho(15);

  printf("s: %d, d: %d\n", t.getValue(), p->getValue());
  return 0;
}
