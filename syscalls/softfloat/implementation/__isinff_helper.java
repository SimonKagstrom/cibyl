public static int __isinff_helper(int _a)
{
  float a = Float.intBitsToFloat(_a);
  int out = Float.isInfinite(a) ? 1 : 0;

  return out;
}
