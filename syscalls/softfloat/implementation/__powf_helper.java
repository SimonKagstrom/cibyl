public static int __powf_helper(int _a, int _b)
{
  float a = Float.intBitsToFloat(_a);
  float b = Float.intBitsToFloat(_b);

#if defined(NOJ2ME)
  return Float.floatToIntBits( (float)Math.pow((double)a, (double)b) );
#else
  /* Not available */
  System.err.println("Using pow, not available in midp");
  return 0;
#endif /* NOJ2ME */

}
