public static int __atanf_helper(int _a)
{
  float a = Float.intBitsToFloat(_a);

  return Float.floatToIntBits( (float)mMath.atan(a) );
}
