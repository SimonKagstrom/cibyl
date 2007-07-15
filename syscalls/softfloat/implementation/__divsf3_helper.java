public static int __divsf3_helper(int _a, int _b)
{
  float a = Float.intBitsToFloat(_a);
  float b = Float.intBitsToFloat(_b);

  //Review: What about div by zero ?
  return Float.floatToIntBits(a / b);
}
