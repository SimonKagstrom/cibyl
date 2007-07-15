public static int __isnanf_helper(int _a)
{
  float a = Float.intBitsToFloat(_a);
  int out = Float.isNaN(a) ? 1 : 0;

  return out;
}
