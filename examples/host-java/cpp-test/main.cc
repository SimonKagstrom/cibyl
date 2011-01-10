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

class Base
{
public:
  virtual void function()
  {
    printf("\n In Base Function");
  }
  virtual void function1()
  {
    printf("\n In Base Function1");
  }
};

class Derived:public Base
{
public:
  void function()
  {
    printf("\n In Derived function");
  }
  void function1()
  {
    printf("\n In Derived function1");
  }
};


int main(int argc, char **argv)
{
  Tjoho t(9);
  Tjoho *p = new Tjoho(15);
  Base *B;
  Derived D;

  B = &D;
  B->function();
  B->function1();

  printf("s: %d, d: %d\n", t.getValue(), p->getValue());
  return 0;
}
