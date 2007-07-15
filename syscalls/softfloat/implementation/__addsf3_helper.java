public static int __addsf3_helper(int _a, int _b)
{
  float a = Float.intBitsToFloat(_a);
  float b = Float.intBitsToFloat(_b);

  return Float.floatToIntBits(a + b);
}
