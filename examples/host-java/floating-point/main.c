#include <stdio.h>
#include <math.h>

float a = 1.9;
float b = 0.2;
double ad = 1.9;
double bd = 0.2;
int tmp;
long tmp_l;
unsigned int tmp_u;
unsigned long tmp_ul;

double scratch;

unsigned long long lla = 0x3ffe666666666666ull;

#include <stdarg.h>
void testme(va_list ap)
{
  scratch = (double)va_arg(ap, double);
}

void maboo(int fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  scratch = (double)va_arg(ap, double);
  va_end(ap);
}

int main(int argc, char* argv[])
{
  tmp = (int)(a + b);
  printf("%d\n", tmp);
  tmp = (int)(a / b);
  printf("%d\n", tmp);
  tmp = (int)(a - b);
  printf("%d\n", tmp);
  tmp = (int)(a * b);
  printf("%d\n", tmp);

  tmp = (int)(ad + bd);
  printf("%d\n", tmp);
  tmp = (int)(ad / bd);
  tmp_l = (long)(ad / bd);
  if (ad >= bd)
    tmp_u = (unsigned int)(ad / bd);
  tmp_ul = (unsigned long)(ad / bd);
  printf("%d:%u:%ld:%lu\n", tmp, tmp_u, tmp_l, tmp_ul);
  tmp = (int)bd;
  printf("%d\n", tmp);
  tmp = (int)(ad - bd);
  printf("%d\n", tmp);
  tmp = (int)(ad * bd);
  printf("%d\n", tmp);

  printf("XXX %f\n", a);

  {
    int i=0;
    float c=0;
    for(i=0; i<1500000; i++)
    {
      float d=0+c;
      float e=1-c;

      float abs_val = sinf(d) + cosf(e);
      abs_val = fabsf(abs_val);

      c = log10f((abs_val + c)) ;
      if (c>10) c=(float)((int)c % (int)10);
      c = powf(c,c);
    }
    printf("c=%f\n", c);
  }
  {
    int i=0;
    double c=0;
    for(i=0; i<1500000; i++)
    {
      double d=0+c;
      double e=1-c;
      c = log10((fabs(sin(d) + cos(e)) + c)) ;
      if (c>10) c=(double)((int)c % (int)10);
      c = pow(c,c);
    }
    printf("cd=%f\n", c);
  }

  printf("blabla %d\n", argc);

  a = 100.0;
  ad = 100.0;

  printf("log(%f) = %f\n", ad, log(ad));
  printf("logf(%f) = %f\n", a, logf(a));
  printf("log10(%f) = %f\n", ad, log10(ad));
  printf("log10f(%f) = %f\n", a, log10f(a));

  a = 8.0;
  ad = 8.0;

  printf("exp(%f) = %f\n", ad, exp(ad));
  printf("expf(%f)= %f\n", a, expf(a));

  b = 4.5;
  bd = 4.5;

  printf("pow(%f, %f) = %f\n", ad, bd, pow(ad, bd));
  printf("powf(%f, %f) = %f\n", a, b, powf(a, b));

  return 0;
}
