public static int __expf_helper(int _a)
{
  float a = Float.intBitsToFloat(_a);

#if defined(NOJ2ME)
  return Float.floatToIntBits( (float)Math.exp(a) );
#else
  /* Not available */
  System.err.println("Using exp, not available in midp");
  return 0;
#endif /* NOJ2ME */

}
