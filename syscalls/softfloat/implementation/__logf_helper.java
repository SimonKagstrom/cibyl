public static int __logf_helper(int _a)
{
  float a = Float.intBitsToFloat(_a);

  /* Not available */
  System.err.println("Using log, not available in midp");
  return 0; // Float.floatToIntBits( (float)Math.log((double)a) );
}
