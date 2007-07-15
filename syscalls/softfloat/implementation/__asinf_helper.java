public static int __asinf_helper(int _a)
{
  float a = Float.intBitsToFloat(_a);

  return Float.floatToIntBits( (float)mMath.asin(a) );
}
