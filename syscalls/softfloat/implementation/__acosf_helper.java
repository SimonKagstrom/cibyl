public static int __acosf_helper(int _a)
{
  float a = Float.intBitsToFloat(_a);

  return Float.floatToIntBits( (float)mMath.acos(a) );
}
