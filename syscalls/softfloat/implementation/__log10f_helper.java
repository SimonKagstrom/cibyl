public static int __log10f_helper(int _a)
{
  float a = Float.intBitsToFloat(_a);

  return Float.floatToIntBits( (float)Math.log10((double)a) );
}
