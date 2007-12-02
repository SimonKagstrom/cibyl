public static int __expf_helper(int _a)
{
  float a = Float.intBitsToFloat(_a);

  /* Not available */
  System.err.println("Using exp, not available in midp");
  return 0;//Float.floatToIntBits( (float)Math.exp(a) );
}
