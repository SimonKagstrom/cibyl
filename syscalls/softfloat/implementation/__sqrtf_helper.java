public static int __sqrtf_helper(int _a)
{
  float a = Float.intBitsToFloat(_a);

  return Float.floatToIntBits( (float)Math.sqrt(a) );
}
