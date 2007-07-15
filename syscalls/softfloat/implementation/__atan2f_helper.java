public static int __atan2f_helper(int _a, int _b)
{
  float a = Float.intBitsToFloat(_a);
  float b = Float.intBitsToFloat(_b);
  double out = mMath.atan2((double)a, (double)b);

  return Float.floatToIntBits( (float)out );
}
