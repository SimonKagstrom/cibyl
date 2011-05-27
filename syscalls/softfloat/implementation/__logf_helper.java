public static int __logf_helper(int _a)
{
  float a = Float.intBitsToFloat(_a);

#if defined(NOJ2ME)
  return Float.floatToIntBits( (float)Math.log((double)a) );
#else
  /* Not available */
  System.err.println("Using log, not available in midp");
  return 0;
#endif /* NOJ2ME */

}
