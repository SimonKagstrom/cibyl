public static int __fixunssfsi_helper(int _a)
{
  float a = Float.intBitsToFloat(_a);

  if (a < 0)
    return 0;

  long tmp = (long)a;

  if (tmp > 0xffffffffl)
    return -1;

  return (int)(tmp & 0xffffffffl);
}
