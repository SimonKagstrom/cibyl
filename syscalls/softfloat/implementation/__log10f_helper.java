public static int __log10f_helper(int _a)
{
  float a = Float.intBitsToFloat(_a);

#if defined(NOJ2ME)
  return Float.floatToIntBits( (float)Math.log10((double)a) );
#else
  /* Not available */
  System.err.println("Using log10, not available in midp");
  return 0
#endif /* NOJ2ME */

}
